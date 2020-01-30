#include <iostream>
#include <fstream>
#include <algorithm>
#include "common/common.hh"
#include "common/Progress.hh"
#include "src/DataFrame.hh"
#include "src/DataBlock.hh"
#include "src/database.hh"

bool run_one_carrier(db::Carrier_t crr, const std::string &dir_file, const std::string &trace_file,
        const std::string &ground_truth, const std::string &logname)
{
    auto *pdbc = db::DatabaseContext::GetDatabaseContext();
    db::Initialize({{crr, dir_file}});

    /* read ground truth */
    using lb = src::Label;
    src::DataFrame gt{ground_truth};
    gt.setLabels({lb::INDEX, lb::LONGTITUDE, lb::LATITIDE, lb::SPEED, 
            lb::HANDOVER, lb::TIME, lb::CELLID});
    //std::cerr<<gt.to_string()<<std::endl;

    /* read trace file and update */
    //common::input_helper helper(trace_file, ' ');
    /* 0,1: gps, 7: time, 9: cid */
    std::cerr.sync_with_stdio(false);
    src::DataFrame df(trace_file);
    int i = std::rand() % (df.rows() - 30);
    LOG_MESSAGE("Testing carrier", crr, "Randomly choose 30 seconds start from", i);
    src::DataFrame result, warn_result;
    bool flag = false;
    common::ProgressBar bar(std::cout, df.rows(), "running");
    int undet_cnt = 0, total_cnt = 0;
    for(auto &db : df.getData())
    {
        bar.increase(1);
        //LOG("Data", db.get(0), db.get(1), db.get(8), db.get(10));
        if(db.get(3) < 220) continue; // low speed
        db::UpdateCellID((db::Cid_t)(db.get(10)), crr);
        db::UpdateGPS(db.get(1), db.get(2), db.get(8));
        pdbc->wait();
        auto val = pdbc->getPrediction(crr);

        auto curr_cell = db.get(10);
        auto curr_time = db.get(8);
        if(!flag)
        {
            auto labels = val.getLabels();
            labels.push_back(src::Label::TIME);
            labels.push_back("error");
            result.setLabels(labels);
            flag = false;
        }

        if(val.rows() > 0 && 
                (db::Cid_t)val.select({lb::CELLID}).getData()[0].get(0) == curr_cell)
        {
            result.getData().push_back(val.getData()[0]);
            result.getData().back().add(db.get(8));

            /* calculate error */
            auto tempdf = gt.select({lb::CELLID, lb::TIME, lb::HANDOVER});
            tempdf = tempdf.where(0, [curr_cell](const double d){return d == curr_cell;})
                            .where(1, [curr_time](const double d){return std::fabs(d - curr_time) < 100;});
            //std::cout<<tempdf.to_string()<<std::endl;
            if(tempdf.rows() != 1) continue;
            auto &vec = tempdf.getData();
            double err = 1e6;
            double err_warn = 1e6;
            for(auto v : vec)
            {
                err = std::min(err, std::fabs(v.get(1) - val.getData()[0].get(0)));
                err_warn = std::min(err_warn, v.get(1) - val.getData()[0].get(3));
            }

            if(tempdf.getData()[0].get(2) != 1) // handover failure
            {
                warn_result.addRow();
                warn_result.getData().back().add(curr_cell);
                warn_result.getData().back().add(err_warn);
            }

            if(err > 1e5) continue;
            result.getData().back().add(err);

        }
        
        //LOG_DEBUG("Val is:");
        //std::cerr<<val.to_string()<<std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    pdbc->wait();
    db::DatabaseContext::DeleteDatabaseContext();
    /* print result to stdout */
    //std::ofstream fout("running/output.log");
    //fout<<result.to_string(false)<<std::endl;
    //fout.close();
    std::ofstream fout2(logname);
    fout2<<warn_result.to_string(false)<<std::endl;
    fout2.close();
    return true;
}

int main(int argc, char *argv[])
{
    run_one_carrier(db::MOBILE, "../m3db-data/dir-Mobile.1", 
            "../m3db-data/datafiles/20191206.5099-Mobile-final.csv.1",
            "../gps_processor/processed/all/20191206.5099-Mobile-dis-start.csv", 
            "running/validation/dir_warning/w1.log");
    run_one_carrier(db::MOBILE, "../m3db-data/dir-Mobile.1", 
            "../m3db-data/datafiles/20191206.5102-Mobile-final.csv.1",
            "../gps_processor/processed/all/20191206.5102-Mobile-dis-start.csv",
            "running/validation/dir_warning/w2.log");
    run_one_carrier(db::MOBILE, "../m3db-data/dir-Mobile.1", 
            "../m3db-data/datafiles/20191208.5102-Mobile-final.csv.1",
            "../gps_processor/processed/all/20191208.5102-Mobile-dis-start.csv",
            "running/validation/dir_warning/w3.log");
    run_one_carrier(db::TELECOM, "../m3db-data/dir-Teleco.1", 
            "../m3db-data/datafiles/20191206.5099-Teleco-final.csv.1",
            "../gps_processor/processed/all/20191206.5099-Teleco-dis-start.csv", 
            "running/validation/dir_warning/w4.log");
    run_one_carrier(db::TELECOM, "../m3db-data/dir-Teleco.1", 
            "../m3db-data/datafiles/20191206.5102-Teleco-final.csv.1",
            "../gps_processor/processed/all/20191206.5102-Teleco-dis-start.csv",
            "running/validation/dir_warning/w5.log");
    run_one_carrier(db::TELECOM, "../m3db-data/dir-Teleco.1", 
            "../m3db-data/datafiles/20191208.5102-Teleco-final.csv.1",
            "../gps_processor/processed/all/20191208.5102-Teleco-dis-start.csv",
            "running/validation/dir_warning/w6.log");
    return 0;
}
