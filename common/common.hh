#ifndef _COMMOM_HH_
#define _COMMOM_HH_

/**
 * common.hh: common utilities here!
 * Include:
 *  Log System
 *  Input helper
 *  to_string() extension
 *  Typedefs
 */

#include <string>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#define SUPPRESS_UNUSED(var) do {static_cast<void>(var);} while(0);
#define LOG_ERROR(...)      common::Log("ERROR"  , __VA_ARGS__, ::combine_ffl(__FILE__, __func__, __LINE__))
#define LOG_MESSAGE(...)    common::Log("MESSAGE", __VA_ARGS__, ::combine_ffl(__FILE__, __func__, __LINE__))
#define LOG_DEBUG(...)      common::Log("DEBUG"  , __VA_ARGS__, ::combine_ffl(__FILE__, __func__, __LINE__))
#define LOG(STR, ...)       common::Log(STR      , __VA_ARGS__, ::combine_ffl(__FILE__, __func__, __LINE__))

namespace common   
{
class input_helper;

std::vector<std::string> split(const std::string &s, char c=' ');
template <typename T1, typename T2> std::string to_string(const std::pair<T1, T2> &p);
template <typename ...Types> void Log(const std::string &prefix, Types ...args);
using std::to_string;
} // namespace common

namespace db
{
using Cid_t = int64_t;
enum Carrier_t { MOBILE = 0, UNICOM = 1, TELECOM = 2};
} // namespace db

namespace   // anonymous namespace
{

template <typename T> void basic_log(T first)
{
    std::cerr<<first;
}
template <typename T, typename ...Types> void basic_log(T first, Types ...args)
{
    std::cerr<<first<<" ";
    basic_log(args...);
}

std::string combine_ffl(const char *file, const char *func, int line)
{
    std::string ret{"-- Function: "};
    ret += func;
    ret += "() in ";
    ret += file;
    ret += ":" + std::to_string(line);
    return ret;
}

}   // anonymous namespace

namespace common
{
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

template <typename T1, typename T2> std::string to_string(const std::pair<T1, T2> &p) { return "{" + std::to_string(p.first) + ", " + std::to_string(p.second) + "}";}

template <typename ...Types> void Log(const std::string &prefix, Types ...args)
{
    std::cerr<<"[ "<<prefix<<" ]: ";
    ::basic_log(args...);
    std::cerr<<std::endl;
}

};

#endif
