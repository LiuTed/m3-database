#include <stdexcept>
#include <typeinfo>
#include <iostream>
#include "Test.hh"

using namespace test;
bool Test::operator()(void) noexcept
{
    bool passed = false;
    try
    {
        passed = this->run();
    }catch (std::exception &e)
    {
        std::string err { e.what() };
        fprintf(stderr, "%s\n", _construct_err(err).c_str());
        return false;
    }

    if(passed)
    {
        fprintf(stderr, "%s\n", _construct_pass().c_str());
        return true;
    }
    else
    {
        fprintf(stderr, "%s\n", _construct_err("run() returned false").c_str());
        return false;
    }
}

std::string Test::getName() const
{
    return typeid(*this).name();
}

std::string Test::_construct_err(const std::string &err) const
{
    std::string name {this->getName()};
    return "\033[31m================ Test " + name + " Failed: " 
        + err + " ================\033[0m";
}

std::string Test::_construct_pass() const
{
    std::string name {this->getName()};
    return "\033[32m================ Test " + name + " Passed! "
        "=================\033[0m";
}

/* ====================== Tester ======================== */
void Tester::enqueue(std::shared_ptr<Test> pt)
{
    vec.push_back(pt);
}

void Tester::enqueue(Test *pt)
{
    this->enqueue(std::shared_ptr<Test>(pt));
}

void Tester::start(bool stopOnFail)
{
    int count = 0;
    for(auto &pt : this->vec)
    {
        auto passed = (*pt)();
        if(stopOnFail && (!passed))
            break;
        count+=1;
    }
    std::string str{std::to_string(count) + "/" + std::to_string(this->vec.size()) + " tests"};
    fprintf(stderr, "\033[32m================ Finished, %s Passed! "
        "=================\033[0m\n", str.c_str());
}


/* ======================== Assert Function ====================*/
void Assert(bool cond, const char *file, int line, const char *func, const char *str)
{
    if(!cond)
    {
        std::string err{"Assertion failed at function "};
        err = err + func + "() in " + file + ":" + std::to_string(line);
        if(str)
            err += " Extra info: " + std::string(str);
        throw std::runtime_error(err);
    }
}
