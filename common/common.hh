#ifndef _COMMOM_HH_
#define _COMMOM_HH_

#include <string>
#include <fstream>
#include <random>
#include <vector>
#include "mkldnn.hpp"

#define SUPPRESS_UNUSED(var) do {static_cast<void>(var);} while(0);

namespace common   
{
using Float_t       = float;    /* type Float for common modules and data modules */
using Float         = Float_t;  /* type Float for common modules and data modules */


std::vector<std::string> split(const std::string &s, char c=' ');
using std::to_string;
template <typename T1, typename T2> 
std::string to_string(const std::pair<T1, T2> &p) 
{ return "{" + std::to_string(p.first) + ", " + std::to_string(p.second) + "}";}

class input_helper
{
    private:
        std::ifstream fin;
        char deli;
    public:
        input_helper(const std::string &s, char d = ',') : fin(s), deli(d)
        {
            if(!fin) throw std::runtime_error("file not found!\n");
            //std::getline(fin,this->header);
        }
        std::vector<std::string> next()
        {
            std::string line{};
            std::getline(fin, line);
            if(line[0] == '#') line = ""; //ignore comments
            return split(line,deli);
        }
        bool hasNext(){return !fin.eof();}
        ~input_helper() {fin.close();}
};

};

namespace test
{
using Float                 = common::Float;
} // namespace test

#endif
