#ifndef _COMMOM_HH_
#define _COMMOM_HH_

#include <string>
#include <random>
#include <vector>
#include "mkldnn.hpp"

#define SUPPRESS_UNUSED(var) do {static_cast<void>(var);} while(0);

namespace common   
{
using Float_t       = float;    /* type Float for common modules and data modules */
using Float         = Float_t;  /* type Float for common modules and data modules */
}

namespace common
{
std::vector<std::string> split(const std::string &s, char c=' ');
using std::to_string;
template <typename T1, typename T2> 
    std::string to_string(const std::pair<T1, T2> &p) 
    { return "{" + std::to_string(p.first) + ", " + std::to_string(p.second) + "}";}
};

namespace test
{
using Float                 = common::Float;
} // namespace test

#endif
