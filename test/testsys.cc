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

