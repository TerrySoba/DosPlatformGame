#include "unit_test.h"
#include "shared_ptr.h"


#include <stdio.h>

struct TestThingy
{
    TestThingy(const char* text) :
        m_text(text)
    {
    }

    ~TestThingy()
    {
        // printf("~TestThingy: %s\n", m_text);
    }

    void doIt() {
        // printf("doIt: %s\n", m_text);
    }

    const char* m_text;
};


TEST(SharedPtrTest1)
{
    shared_ptr<int> i(new int);
    *(i.get()) = 1;
    ASSERT_TRUE(*i == 1);
    ASSERT_TRUE(i.use_count() == 1);
    shared_ptr<int> n(i);
    ASSERT_TRUE(i.get() == n.get());
    ASSERT_TRUE(i.use_count() == 2);
}

TEST(SharedPtrTest2)
{
    shared_ptr<TestThingy> i(new TestThingy("test1"));
    shared_ptr<TestThingy> j = i;

    i = new TestThingy("test2");

    j->doIt();
    i->doIt();
}

TEST(SharedPtrTest3)
{
    shared_ptr<TestThingy> i(new TestThingy("test1"));
    shared_ptr<TestThingy> j = i;

    i = new TestThingy("test2");
    j = new TestThingy("test3");

    j->doIt();
    i->doIt();
}

TEST(SharedPtrTest4)
{
    shared_ptr<TestThingy> i(new TestThingy("test1"));
    shared_ptr<TestThingy> j = i;
    i.reset();
    j.reset();
    
}
