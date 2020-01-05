#include <sstream>
#include "common.hh"

namespace common
{
std::vector<std::string> split(const std::string &s, char c)
{
    std::stringstream ss;
    ss<<s;
    std::string temp;
    std::vector<std::string> ret;
    while(std::getline(ss, temp, c))
    {
        if(temp.size()) ret.emplace_back(temp);
    }
    return ret;
}
};
