#include "testcommon.hh"
#include "../src/database.hh"
#include <functional>
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

    common::Log("TEST", 1,2,3,"hello world");
    LOG_ERROR("test log error");
    LOG_MESSAGE("test log message");
    LOG("EVERYTHING", "test log");

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

/* ============ DataFrame test =========== */
bool DataFrameTest::run()
{
    using lb = src::Label;
    src::DataFrame df("running/test_df.csv"); // test constructor
    /* test setLabels and select */
    src::DataFrame df2 = df.setLabels({lb::INDEX, lb::LONGTITUDE, lb::LATITIDE, lb::SPEED, 
            lb::THROUGHPUT, lb::RTT, lb::LOSS, lb::RSRP, lb::TIME, 
            lb::HANDOVER, lb::CELLID}).select({lb::RTT, lb::RSRP, lb::THROUGHPUT});
    /* test rows, columns, getLabels */
    ASSERT_EQUAL((int)df2.columns(), 3);
    ASSERT_EQUAL(df2.rows(), df.rows());
    ASSERT(df2.getLabel(1) == lb::RSRP);
    
    /* test where */
    auto df3 = df.where(0, [](double d){return d > 5 && d < 10;});
    ASSERT(df3.rows() == 4); // row 6,7,8,9

    auto df4 = df.where(df.getColumn(lb::HANDOVER), [](double b){return std::round(b) > 0;})
                 .where(df.getColumn(lb::LOSS), [](double l){return l > 0;});
    auto df5 = df.where<std::function<bool(double)>>({df.getColumn(lb::HANDOVER), df.getColumn(lb::LOSS)},
                        {[](double h){return std::round(h) > 0;},
                         [](double l){return l > 0;}});
    auto col1 = df.getColumn(lb::HANDOVER), col2 = df.getColumn(lb::LOSS);
    auto df6 = df.where([col1, col2](const src::Datablock &b){return std::round(b.get(col1)) > 0 && b.get(col2) > 0;});
    ASSERT_EQUAL(df4.rows(), df5.rows());
    ASSERT_EQUAL(df4.rows(), df6.rows());
    ASSERT(df4.to_string() == df5.to_string());
    ASSERT(df4.to_string() == df6.to_string());

    /* test extend */
    ASSERT_FAULT(df2.extend(df));
    ASSERT_EQUAL(df4.extend(df5).rows(), df5.rows() + df6.rows());

    df4.addRow();
    df4.addColumn("Test", 9999);
    std::cerr<<df.to_string()<<std::endl;
    std::cerr<<df2.to_string()<<std::endl;
    std::cerr<<df3.to_string()<<std::endl;
    std::cerr<<df4.to_string()<<std::endl;
    return true;
}

/* ============ Queue test =========== */
bool QueueTest::run() 
{
    using namespace std::chrono_literals;
    src::ConsumerProducerQueue<int> queue;
    std::vector<int> src, dst;
    for(int i=0;i<200;i++) src.push_back(std::rand());
    auto prod = [](src::ConsumerProducerQueue<int> &q, const std::vector<int> &src){
                    for(unsigned i=0;i<src.size();i++)
                    {
                        q.add(src[i]);
                        std::this_thread::sleep_for(1ms);
                    }
                };
    auto cons =  [](src::ConsumerProducerQueue<int> &q, std::vector<int> &dst){
                     for(int i=0;i<200;i++)
                     {
                         dst.push_back(q.pop());
                     }
                 };

    std::thread producer(prod, std::ref(queue), std::ref(src));
    std::thread consumer(cons, std::ref(queue), std::ref(dst));

    producer.join();
    consumer.join();

    ASSERT(std::equal(src.begin(), src.end(), dst.begin()));
    return true;
}
