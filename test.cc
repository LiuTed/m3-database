#include "common/common.hh"
#include "common/Test.hh"
#include "test/testcommon.hh"


int main()
{
    test::Tester tester;

    tester.emplace<test::CommonTest>();
    tester.emplace<test::DatablockTest>();

    tester.start(true);
}
