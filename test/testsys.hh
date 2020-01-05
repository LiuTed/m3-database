#ifndef _TESTSYS_HH_
#define _TESTSYS_HH_

#include "../common/Test.hh"

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

} // namespace test

#endif
