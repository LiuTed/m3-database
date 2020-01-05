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

} // test

#endif
