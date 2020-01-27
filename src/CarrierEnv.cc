#include <functional>
#include <algorithm>
#include "CarrierEnv.hh"
#include "../common/Progress.hh"

using Loc_t = std::pair<double, double>;    // first is lng, second is lat
/* ===== helpers ===== */
/**
 * getDistance, return the estimated distance (meters) between two location
 */
static double getDistance(const Loc_t &l1, const Loc_t &l2);

/**
 * findNearest, find the nearest location in a dataframe
 */
static src::Datablock findNearest(const Loc_t &loc, const src::DataFrame &df);

/**
 * struct LocCmp
 * used to compare the distance between a specified location
 */
struct LocCmp
{
    Loc_t point;
    LocCmp(const Loc_t &loc) : point(loc) {}
    bool operator()(const Loc_t &l, const Loc_t &r) const
    {
        return getDistance(l, point) < getDistance(r, point);
    }
};


namespace src
{

CarrierEnv::CarrierEnv(const std::vector<std::string> &files,
        const std::vector<std::string> &ho_files,
        const std::vector<std::string> &bound_files)
{
    using lb = src::Label;
    LOG_DEBUG("CarrierEnv: Got ", files.size(), "data files");
    LOG_DEBUG("CarrierEnv: Got ", ho_files.size(), "handover files");
    LOG_DEBUG("CarrierEnv: Got ", bound_files.size(), "boundary files");
    if(ho_files.size() > 1)
    {
        LOG_ERROR("CarrierEnv: can only have at most 1 handover file to initialize!");
        throw std::runtime_error("CarrierEnv: can only have at most 1 handover file to initialize");
    }

    common::ProgressBar bar(std::cerr, 
            files.size() + ho_files.size() + bound_files.size(), "Reading");
    for(auto &file: files)
    {
        bar.increase(1);
        df.emplace_back(file);
        df.back().setLabels({lb::INDEX, lb::LONGTITUDE, lb::LATITIDE, lb::SPEED,
                lb::THROUGHPUT, lb::RTT, lb::LOSS, lb::RSRP, lb::TIME,
                lb::HANDOVER, lb::CELLID});
        //LOG_DEBUG("CarrierEnv: File:", file, ", get", df.back().rows(), "records");
    }

    for(auto &file : ho_files)
    {
        ho_df = DataFrame(file);
        ho_df.setLabels({lb::INDEX, lb::LONGTITUDE, lb::LATITIDE, lb::RSRP,
                lb::HANDOVER, lb::CELLID});
        //LOG_DEBUG("CarrierEnv: File:", ho_files[0], ", get", ho_df.rows(), "records");
    }

    /* TODO: initialize boundary dataFrame */
    for(auto &file : bound_files)
    {
        bd_df = DataFrame(file);
        bd_df.setLabels({lb::INDEX, lb::CELLID, lb::LONGTITUDE, lb::LNG_CFDS,
                lb::LATITIDE, lb::LAT_CFDS, lb::BOUND_STATUS});
        //LOG_DEBUG("CarrierEnv: File:", ho_files[0], ", get", ho_df.rows(), "records");
    }
    LOG("WARNING", "CarrierEnv: Need to implement boundary DataFrame initialization");
    this->tcp_prediction.setLabels({lb::TIME, lb::THROUGHPUT, lb::RTT, lb::LOSS, lb::HANDOVER});
    this->ho_prediction.setLabels({lb::HOTIME, lb::CELLID, lb::SUCCESS_RATE, lb::FAIL_TIME});
    this->ho_prediction.addRow();   // reserve a row to store the results
    current_cell = 0;
}

void CarrierEnv::updateCell(Cid_t new_cell)
{
    this->current_cell = new_cell;
}

DataFrame CarrierEnv::getPrediction()
{
    DataFrame ret;
    {
        std::unique_lock<std::mutex> lock(this->pred_lock);
        ret = this->ho_prediction;
    }
    return ret; 
}

void CarrierEnv::updateLocation(double lng, double lat, double time)
{
    LOG_MESSAGE("CarrierEnv::updateLocation", lng, lat, time, "df size is", df.size());
    //this->predict_tcp(lng, lat, time);
    this->predict_handover(lng, lat, time);
    prev_location = {lng, lat, time};
}

void CarrierEnv::predict_handover(double lng, double lat, double time)
{
    //throw std::runtime_error("CarrierEnv::predict_handover: not implemented!");
    double v_lng = (lng - prev_location.lng) / (time - prev_location.time);
    double v_lat = (lat - prev_location.lat) / (time - prev_location.time);

    /* use cell id to find the next handover location */
    double next_lng, next_lat;
    /* select the current cell id */
    auto cid_col = this->ho_df.getColumn(Label::CELLID),
         lng_col = this->ho_df.getColumn(Label::LONGTITUDE),
         lat_col = this->ho_df.getColumn(Label::LATITIDE),
         ho_col = this->ho_df.getColumn(Label::HANDOVER);

    auto cell_df = this->ho_df.where(cid_col, [this](const double d){return int(d) == this->current_cell;});
    auto cell_df1 = cell_df.where(ho_col, [](const double d){return d == 1;});
    //auto cell_df2 = cell_df.where(ho_col, [](const double d){return d > 1;});
    double succ_rate = (0. + cell_df1.rows() * 1.0) / (0. + cell_df.rows());
    if(cell_df.rows() == 0)
    {
        LOG("WARNING", "CarrierEnv::predict_handover: no cell handover information!");
        return;
    }
    auto avg_df = DataFrameHelper::GetAverage(cell_df);
    //LOG_DEBUG("\n",avg_df.to_string());

    next_lng = avg_df.getData()[0].get(lng_col);
    next_lat = avg_df.getData()[0].get(lat_col);

    /* use speed to calculate time:
     * let X = (delta_x, delta_y), 
     * let Y = (speed_x, speed_y), --> speed = |Y|
     * time = proj / |Y|, where proj = |project X to Y|
     *      = |X| * cos<X,Y> / |Y|
     *      = |X| * (X·Y) / (|X| * |Y|) / |Y|
     *      = (X·Y) / Y^2
     *      = [(delta_x * speed_x) + (delta_y * speed_y)] / 
     *                  (speed_x * speed_x + speed_y * speed_y)
     */
    double delta_lng = next_lng - lng,
           delta_lat = next_lat - lat;
    double remain_time = ((delta_lng * v_lng) + (delta_lat * v_lat)) / 
        (v_lng * v_lng + v_lat * v_lat);
    LOG_DEBUG("CarrierEnv::predict_handover: remain time is: ", remain_time);

    /* handover failure prediction */
    double fail_time = 86400;
    cid_col = this->bd_df.getColumn(Label::CELLID);
    lng_col = this->bd_df.getColumn(Label::LONGTITUDE),
    lat_col = this->bd_df.getColumn(Label::LATITIDE);
    auto sta_col = this->bd_df.getColumn(Label::BOUND_STATUS),
         lnc_col = this->bd_df.getColumn(Label::LNG_CFDS),
         lac_col = this->bd_df.getColumn(Label::LAT_CFDS);

    cell_df = this->bd_df.where(cid_col, [this](const double d){return int(d) == this->current_cell;});

    double lng_cfds, lat_cfds, fail_lng, fail_lat;
    int status;
    if(cell_df.rows() == 0) 
        goto FINAL;
    status = (int)cell_df.getData()[0].get(sta_col);
    lng_cfds = cell_df.getData()[0].get(lnc_col);
    lat_cfds = cell_df.getData()[0].get(lac_col);
    fail_lng = cell_df.getData()[0].get(lng_col);
    fail_lat = cell_df.getData()[0].get(lat_col);
    /**
     * if status == -1: all fail, fail_time equals to time + remain_time
     * if status == 1: all success, fail_time equals to 86400
     * if status == 0: need check:
     *  if LNG_CFDS == 1., fail_time = time + (LNG - currlng) / lng_spd
     *  else if LAT_CFDS == 1., fail_time = time + (LAT - currlat) / lat_spd
     *  else cannot determin, fail time equals to time + remain_time + 3
     */
    if(status == -1) // all fail
    {
        fail_time = time + remain_time;
    }
    else if (status == 1) // all success
    {
        fail_time = 86400.0;
    }
    else 
    {
        if(lng_cfds == 1) 
            fail_time = time + (fail_lng - lng) / v_lng;
        else if (lat_cfds == 1)
            fail_time = time + (fail_lat - lat) / v_lat;
        else 
            fail_time = time + remain_time + 2;
    }


    /* modify the prediction result */
FINAL:;
    {
        std::unique_lock<std::mutex> lk(pred_lock);
        /* set prediction to 0 */
        this->ho_prediction.getData()[0].set(0, time + remain_time);
        this->ho_prediction.getData()[0].set(1, this->current_cell);
        this->ho_prediction.getData()[0].set(2, succ_rate);
        this->ho_prediction.getData()[0].set(3, fail_time);
    }
    //LOG_ERROR("CarrierEnv::predict_handover: not implemented!");
}



void CarrierEnv::predict_tcp(double lng, double lat, double time)
{
    const static double SAME_LOCATION_THRESHOLD = 100; // if more than 100m, 2 locations are different location
    const static double MATCH_LENGTH = 5;   // get [t, t+5)
    /* for each day, get the nearest location point */
    /* for each day, get the [0, 5] second data */
    /* collect all of them into one DataFrame, add a column "day" */
    using lb = src::Label;

    Loc_t curr_loc{lng, lat};
    Cid_t curr_cell = this->current_cell;

    /* dataFrame for all matched data */
    int total_matched_day = 0;
    DataFrame allFrame;
    allFrame.setLabels(this->df[0].getLabels());
    allFrame.addColumn("day", 0);


    for(unsigned i = 0; i < df.size(); i++) // for each day
    {
        auto &curr_df = this->df[i];
        auto col_lng = curr_df.getColumn(lb::LONGTITUDE),
             col_lat = curr_df.getColumn(lb::LATITIDE),
             col_time = curr_df.getColumn(lb::TIME),
             col_cell = curr_df.getColumn(lb::CELLID);

        auto block = curr_df.where([=](const Datablock &b)
                {
                auto ln = b.get(col_lng),
                la = b.get(col_lat);
                return std::fabs(lng - ln) < 0.2 && std::fabs(lat - la) < 0.2;
                });

        /* now, linear search for the nearest point in block, and get its time t */
        if (block.rows() == 0) continue;    // skip if no data
        //LOG_DEBUG("small block size is: ", block.rows());
        auto nearest_record = findNearest(curr_loc, block); // it is the nearest point
        Loc_t nearest_loc{nearest_record.get(col_lng), nearest_record.get(col_lat)};
        if(getDistance(nearest_loc, curr_loc) > SAME_LOCATION_THRESHOLD 
                || nearest_record.get(col_cell) != curr_cell)
            continue; // if too far-away or not in one cell, we skip it
        auto start_time = nearest_record.get(col_time);

        /* now, get the [t, t+5) second data */
        auto next_5sec = curr_df.where([start_time, col_time](const Datablock &b)
                {
                return //b.get(col_cell) == curr_cell && 
                b.get(col_time) < start_time + MATCH_LENGTH &&
                b.get(col_time) >= start_time;
                });

        /* change the time from [t, t+5) to [0, 5) */
        auto &temp_vec = next_5sec.getData();
        for(auto &db : temp_vec) 
            db.set(col_time, db.get(col_time) - start_time);

        /* add the column: "day" */
        next_5sec.addColumn("day", i);
        allFrame.extend(next_5sec);
        total_matched_day += 1;
    }

    LOG_MESSAGE("Total Matched day:", total_matched_day);


    /* now, we should make prediction based on allFrame */
    /* thp[0:5] = daily average thp[0:5], Byte per sec*/
    /* rtt[0:5] = daily average ..., second*/
    /* loss[0:5] = daily average ..., 0~1 */
    /* handver[0:5] = daily average ... (1 means always handover, 0 means no handover) */
    DataFrame temp_pred;
    temp_pred.setLabels(this->tcp_prediction.getLabels());

    for(int t = 0; t < MATCH_LENGTH; t++)   // for each second
    {
        temp_pred.addRow();
        temp_pred.getData().back().set(temp_pred.getColumn(lb::TIME), time + t); // update time

        auto time_col = allFrame.getColumn(lb::TIME);
        auto tdf = allFrame.where(time_col, [t](double v){return std::round(v) == t;})
            .select({lb::THROUGHPUT, lb::RTT, lb::LOSS, lb::HANDOVER});

        for(auto label : tdf.getLabels())   // for each type
        {
            auto label_df = tdf.select({label});
            double sum = 0;
            for(auto &record : label_df.getData()) 
            {
                if(label == lb::HANDOVER)
                    sum += !!((int)record.get(0));  // convert 0~5 value to binary value (0 or 1)
                else
                    sum += record.get(0);
            }
            sum /= label_df.rows();
            temp_pred.getData().back().set(temp_pred.getColumn(label), sum); //update the average
        }
    }

    /* lock and update the label */
    {
        std::unique_lock<std::mutex> lock(this->pred_lock);
        this->tcp_prediction = temp_pred;
    }
}

}   // namespace src

/* helpers */
static double getDistance(const Loc_t &l1, const Loc_t &l2)
{
    const double r = 6371.0 * 1000; // in meter
    const double pi = 2 * std::asin(1.);
    auto delta_lng = l1.first - l2.first;
    auto delta_lat = l1.second - l2.second;
    auto y = 2 * pi * r * delta_lat / 360;
    auto x = 2 * pi * (r * std::cos(pi * l1.second / 180.)) * delta_lng / 360;
    return std::sqrt(x * x + y * y);
}

static src::Datablock findNearest(const Loc_t &loc, const src::DataFrame &df)
{
    src::DataFrame ret;
    ret.setLabels(df.getLabels());

    std::vector<Loc_t> vec;
    auto col1 = df.getColumn(src::Label::LONGTITUDE),
         col2 = df.getColumn(src::Label::LATITIDE);

    for(auto &v : df.getData())
        vec.push_back({v.get(col1), v.get(col2)});

    std::sort(vec.begin(), vec.end(), LocCmp(loc));
    auto &target = vec[0];

    return df.where([target, col1, col2](const src::Datablock &b)
            {
            return b.get(col1) == target.first && b.get(col2) == target.second;
            }).getData()[0];
}
