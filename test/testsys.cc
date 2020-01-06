#include "testsys.hh"
#include "../src/Serializeable.hh"
#include "../src/database.hh"
#include "../src/DataBlock.hh"
#include "../src/DataFrame.hh"
#include "../src/Queue.hh"
#include "../src/CarrierEnv.hh"
#include "../src/DatabaseContext.hh"


using namespace test;

/* ================== CarrierEnvTest ================== */
bool CarrierEnvTest::run() 
{
    common::input_helper helper("running/dir-Mobile.txt");
    std::vector<std::string> filenames;

    while(helper.hasNext()) 
    {
        auto vec = helper.next();
        if(vec.empty()) continue;
        filenames.push_back(vec[0]);
    }
    src::CarrierEnv env(filenames);

    src::Cid_t cid = 229816326;
    env.updateCell(cid);
    env.updateLocation(117.314079, 34.184841,0);

    cid = 220359687;
    env.updateCell(cid);
    env.updateLocation(118.733131, 31.932112, 100);

    return true;
}

/* ================== DBContextTest ================== */
bool DBContextTest::run()
{
    auto *pdbc = db::DatabaseContext::GetDatabaseContext();

    /* test initialize */
    db::Initialize({{db::MOBILE, "running/dir-Mobile.txt"},
                    {db::UNICOM, "running/dir-Unicom.txt"},
                    {db::TELECOM, "running/dir-Telecom.txt"}});

    src::Cid_t cid = 229816326;
    db::UpdateCellID(cid, db::MOBILE);
    db::UpdateGPS(117.314079, 34.184841,0);
    pdbc->wait();
    db::DatabaseContext::DeleteDatabaseContext();
    return true;
}

/* ================== End2EndTest ================== */
bool End2EndTest::run_one_carrier(db::Carrier_t crr, const std::string &dir_file,
        const std::string &trace_file)
{
    auto *pdbc = db::DatabaseContext::GetDatabaseContext();
    db::Initialize({{crr, dir_file}});
    /* read trace file and update */
    //common::input_helper helper(trace_file, ' ');
    /* 0,1: gps, 7: time, 9: cid */
    std::cerr.sync_with_stdio(false);
    src::DataFrame df(trace_file);
    int i = std::rand() % (df.rows() - 30);
    LOG_MESSAGE("Testing carrier", crr, "Randomly choose 30 seconds start from", i);
    df = df.where(df.getColumn(src::Label::INDEX), [i](double ind){return std::round(ind) >= i && ind < i + 30;});
    for(auto &db : df.getData())
    {
        //LOG("Data", db.get(0), db.get(1), db.get(8), db.get(10));
        db::UpdateCellID((db::Cid_t)(db.get(10)), crr);
        db::UpdateGPS(db.get(1), db.get(2), db.get(8));
        pdbc->wait();
        auto val = pdbc->getPrediction(crr);
        LOG_DEBUG("Val is:");
        std::cerr<<val.to_string()<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    pdbc->wait();
    db::DatabaseContext::DeleteDatabaseContext();
    return true;
}

bool End2EndTest::run()
{
    bool ret = true;

    ret = run_one_carrier(db::MOBILE, "running/dir-Mobile.txt", "running/testtrace-Mobile.csv");
    if(!ret)
    { 
        LOG_ERROR("Mobile test does not pass!");
        return false;
    }

    ret = run_one_carrier(db::UNICOM, "running/dir-Unicom.txt", "running/testtrace-Unicom.csv");
    if(!ret)
    { 
        LOG_ERROR("Unicom test does not pass!");
        return false;
    }

    ret = run_one_carrier(db::TELECOM, "running/dir-Telecom.txt", "running/testtrace-Telecom.csv");
    if(!ret)
    { 
        LOG_ERROR("Telecom test does not pass!");
        return false;
    }
    
    LOG_MESSAGE("All passed!");
    return true;
}
