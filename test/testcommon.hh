#ifndef _TESTCOMMON_HH_
#define _TESTCOMMON_HH_

#include "../common/Test.hh"
#include "../common/common.hh"

namespace test
{
class CommonTest : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "CommonTest";
        }

        virtual bool run() override;
};

class DatablockTest : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "Datablock Test";
        }

        virtual bool run() override;
};

class DataFrameTest : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "Data Frame Test";
        } 

        virtual bool run() override;
};

class QueueTest : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "Producer Consumer Queue Test";
        }
        
        virtual bool run() override;
};

} // test

#endif
