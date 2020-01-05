#include "testcommon.hh"
#include "../src/database.hh"
#include <iostream>


using namespace test;

/* ============ Common helpers test =========== */
bool CommonTest::run() 
{
    // Testing split
    std::string a, b, c;
    a = "a b c d e";
    auto vec = common::split(a);
    ASSERT(vec.size() == 5);
    vec = common::split(a, 'c');
    ASSERT(vec.size() == 2);
    ASSERT(vec[0].length() == vec[1].length());
    ASSERT(vec[0] == "a b ");
    a = "a    b";
    vec = common::split(a, ' ');
    ASSERT(vec.size() == 2);
    ASSERT(vec[1] == "b");

    //
    return true;
}

/* =============== Datablock test =============== */
bool DatablockTest::run()
{
    src::Datablock b1, b2, b3;
    for(int i=0;i<10;i++) b1.add(i);
    for(int i=0;i<10;i++) b2.add(5);
    b3 = b1.select({1,3,5,7,9});
    ASSERT_EQUAL((unsigned)b3.size(), 5u);
    for(unsigned i=0;i<b3.size();i++)
        ASSERT_EQUAL((int)(b3.get(i)), (int)i * 2 + 1);
    for(unsigned i = 0; i < b1.size(); i++)
    {
        src::less<src::Datablock> ls(i);
        if(i < 5) ASSERT(ls(b1, b2));
        else ASSERT(!ls(b1, b2));
    }
    return true;
}

/* ============ Dimension test =========== */
