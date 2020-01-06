#ifndef _TESTSYS_HH_
#define _TESTSYS_HH_

#include "../common/Test.hh"
#include "../src/database.hh"
#include "../src/Serializeable.hh"
#include "../src/DataBlock.hh"
#include "../src/DataFrame.hh"
#include "../src/Queue.hh"
#include "../src/CarrierEnv.hh"
#include "../src/DatabaseContext.hh"

namespace test
{

class CarrierEnvTest : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "CarrierEnv Test";
        }

        virtual bool run() override;
};

class DBContextTest : public Test
{
    public:
        virtual std::string getName() const override
        {
            return "Database Context Test";
        }

        virtual bool run() override;
};

} // namespace test

#endif
