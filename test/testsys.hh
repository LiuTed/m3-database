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

class End2EndTest : public Test
{
    private:
        bool run_one_carrier(db::Carrier_t crr, const std::string &dir_file, 
                const std::string &trace_file);
    public:
        virtual std::string getName() const override
        {
            return "Database end to end test";
        }

        virtual bool run() override;
};

} // namespace test

#endif
