#include <iostream>
#include <fstream>
#include <algorithm>
#include "common/common.hh"
#include "src/DataFrame.hh"
#include "src/DataBlock.hh"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        LOG_ERROR("Usage:", argv[0], "<total disconnection file>", "<output prefix>");
        exit(-1);
    }

    src::DataFrame df(argv[1]);
    /**
     * df format:
     *  <index> <lng> <lat> <spd> <majtype> <daytime> <cid>
     */

    df.setLabels({"index", "lng", "lat", "spd", "major type", "daytime", "cid"});
    auto cid_blocks = df.getData();
    std::vector<int64_t> cid_vec;
    for(auto cb : cid_blocks)
        cid_vec.push_back(cb.get(6));

    std::sort(cid_vec.begin(), cid_vec.end());
    auto it = std::unique(cid_vec.begin(), cid_vec.end());
    cid_vec.resize(std::distance(cid_vec.begin(), it));

    int cnt = 0;
    for(unsigned i=0;i<cid_vec.size();i++)
    {
        auto cid = cid_vec[i];
        auto temp_df = df.where([cid](const src::Datablock &d){
                    // cid: 6, lng: 2, lat: 3, daytime: 5
                    return d.get(6) == cid &&
                        d.get(2) < 32.54 &&
                        d.get(2) > 32.46 &&
                        d.get(5) < 57600;
                });
        if(temp_df.rows() < 5) continue;
        std::ofstream fout(std::string(argv[2]) + std::to_string(cnt));
        fout<<temp_df.to_string(false)<<std::endl;
        cnt += 1;
    }
    LOG_MESSAGE("Got count: ", cnt);
}

