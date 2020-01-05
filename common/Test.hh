#ifndef _TEST_HH_
#define _TEST_HH_

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "../common/common.hh"

#define TEST(obj) (obj)()
#define ASSERT(cond) Assert(cond, __FILE__, __LINE__, __func__, NULL)
#define ASSERT_WITH(cond, str) Assert(cond, __FILE__, __LINE__, __func__, str)
#define ASSERT_FAULT(expr) AssertFault(expr, __FILE__, __LINE__, __func__)
#define ASSERT_EQUAL(l, r) AssertEqual(l, r, __FILE__, __LINE__, __func__)

#define AssertFault(e, fi, li, fu)  \
    try{ \
        e; throw std::string("");\
    }catch(std::exception &exc){std::cerr<<"Assert-fault: got fault: "<<exc.what()<<", passed"<<std::endl;} \
    catch(std::string &s){throw std::runtime_error("Assertion failed at function " + std::string(fu) + \
    "() in " + fi + ":" + std::to_string(li) + " expression \'" #e "\' should fail but it did not");}

void Assert(bool cond, const char *f, int l, const char *fu, const char *extra);
template<typename T> void AssertEqual(T l, T r, const char *file, int line, const char *func)
{
    std::equal_to<T> eq;
    if(!eq(l, r))
    {
        std::string err{"Assertion failed at function "};
        err = err + func + "() in " + file + ":" + std::to_string(line);
        err += " Expected " + common::to_string(l) + " but got " + common::to_string(r);
        throw std::runtime_error(err);
    }
}


namespace test
{


class Test
{
    private:
        std::string _construct_err(const std::string &err) const;
        std::string _construct_pass() const;
    public:
        virtual std::string getName() const;
        virtual bool run() = 0;
        virtual bool operator()(void) noexcept final;
        virtual ~Test() = default;
};

class Tester
{
    private:
        std::vector<std::shared_ptr<Test>> vec;
    public:
        Tester() = default;

        /**
         * Method enqueue
         * Enqueue a test object
         */
        void enqueue(std::shared_ptr<Test> pt);
        void enqueue(Test *pt);

        /**
         * Method emplace
         * emplace a test object
         */
        template<typename T, typename... Args>
            void emplace(Args &&... args)
            {
                this->enqueue(std::make_shared<T>(args...));
            }

        /**
         * Method start
         * Start the tests. 
         * If stopOnFail is set to true, it will stop when a test is failed
         * @params: stopOnFail -- default is true
         */
        void start(bool stopOnFail = true);

};


} //namespace test

#endif
