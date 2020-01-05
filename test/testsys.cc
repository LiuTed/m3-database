#include "testsys.hh"
#include "../src/database.hh"

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
