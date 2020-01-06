#include "common/common.hh"
#include "common/Test.hh"
#include "test/testcommon.hh"
#include "test/testsys.hh"


int main()
{
    test::Tester tester;

    tester.emplace<test::CommonTest>();
    tester.emplace<test::DatablockTest>();
    tester.emplace<test::DataFrameTest>();
    tester.emplace<test::QueueTest>();
    tester.emplace<test::CarrierEnvTest>();
    tester.emplace<test::DBContextTest>();
    tester.emplace<test::End2EndTest>();

    tester.start(true);
}
