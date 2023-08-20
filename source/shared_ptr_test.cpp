#include "unit_test.h"
#include "shared_ptr.h"
#include "test_class.h"
#include "vector.h"
#include "exception.h"

#include <stdio.h>
#include <string>

using namespace tnd;

static bool s_destructorRan = false;
static int s_instanceCount = 0;

struct TestThingy
{
    TestThingy(const char* text) :
        m_text(text)
    {
        ++s_instanceCount;
    }

    ~TestThingy()
    {
        printf("~TestThingy: %s\n", m_text);
        s_destructorRan = true;
        --s_instanceCount;
    }

    void doIt() {
        printf("doIt: %s\n", m_text);
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

TEST(SharedPtrDestructorTest)
{
    s_destructorRan = false;
    {
        shared_ptr<TestThingy> i(new TestThingy("test1"));
    }
 
    ASSERT_TRUE(s_destructorRan);
}

TEST(SharedPtrVectorTest)
{
    shared_ptr<vector<int> > vec(new vector<int>());
    vec->push_back(3);
    vec->push_back(4);
    vec->push_back(7);
    ASSERT_TRUE(vec->size() == 3);

    shared_ptr<vector<int> > vec2(vec);
    ASSERT_TRUE((*vec)[0] == (*vec2)[0]);
    ASSERT_TRUE((*vec)[1] == (*vec2)[1]);
    ASSERT_TRUE((*vec)[2] == (*vec2)[2]);
    ASSERT_TRUE(vec2->size() == 3);

    shared_ptr<vector<int> > vec3;
    vec3 = vec;
    ASSERT_TRUE((*vec)[0] == (*vec3)[0]);
    ASSERT_TRUE((*vec)[1] == (*vec3)[1]);
    ASSERT_TRUE((*vec)[2] == (*vec3)[2]);
    ASSERT_TRUE(vec3->size() == 3);
}

TEST(SharedPtrVectorTest2)
{
    ASSERT_TRUE(s_instanceCount == 0);
    {
        vector<shared_ptr<TestThingy> > vec;

        for (int i = 0; i< 100; ++i)
        {
            vec.push_back(new TestThingy("test1"));
            vec.push_back(new TestThingy("test2"));
            vec.push_back(new TestThingy("test3"));
            ASSERT_TRUE(vec.size() == 3);
            s_destructorRan = false;
            vec.clear();
            // ASSERT_TRUE(s_destructorRan);
            ASSERT_TRUE(vec.size() == 0);
        }

        printf("done\n");
        s_destructorRan = false;
    }
    // ASSERT_FALSE(s_destructorRan);
    ASSERT_TRUE(s_instanceCount == 0);
}


TEST(SharedPtrExternalTest)
{
    s_instanceCount2 = 0;
    ASSERT_TRUE(s_instanceCount2 == 0);


    shared_ptr<TestThingy2> i;
    ASSERT_TRUE(i.use_count() == 0);
    {
        i = createTestThingy2("test1");
        ASSERT_TRUE(i.use_count() == 1);
        ASSERT_TRUE(s_instanceCount2 == 1);
        shared_ptr<TestThingy2> i2 = i;
        ASSERT_TRUE(i.use_count() == 2);
        ASSERT_TRUE(i2.use_count() == 2);
        ASSERT_TRUE(s_instanceCount2 == 1);
        i = createTestThingy2("test2");
        ASSERT_TRUE(s_instanceCount2 == 2);
        i->doIt();
        ASSERT_TRUE(s_instanceCount2 == 2);
    }

    ASSERT_TRUE(s_instanceCount2 == 1);
}

// a unit test that checks the tnd::shared_ptr class thoroughly
TEST(SharedPtrTestAuto)
{
    {
    shared_ptr<std::string> i(new std::string);
    *i = "0";
    ASSERT_TRUE(i.use_count() == 1);
    ASSERT_TRUE(*i == "0");
    *i = "1";
    ASSERT_TRUE(*i == "1");
    ASSERT_TRUE(i.use_count() == 1);
    shared_ptr<std::string> j = i;
    ASSERT_TRUE(i.use_count() == 2);
    ASSERT_TRUE(j.use_count() == 2);
    ASSERT_TRUE(*i == "1");
    ASSERT_TRUE(*j == "1");
    *i = "2";
    ASSERT_TRUE(*i == "2");
    ASSERT_TRUE(*j == "2");
    ASSERT_TRUE(i.use_count() == 2);
    ASSERT_TRUE(j.use_count() == 2);
    shared_ptr<std::string> k = j;
    ASSERT_TRUE(i.use_count() == 3);
    ASSERT_TRUE(j.use_count() == 3);
    ASSERT_TRUE(k.use_count() == 3);
    ASSERT_TRUE(*i == "2");
    ASSERT_TRUE(*j == "2");
    ASSERT_TRUE(*k == "2");
    *i = "3";
    ASSERT_TRUE(*i == "3");
    ASSERT_TRUE(*j == "3");
    ASSERT_TRUE(*k == "3");
    ASSERT_TRUE(i.use_count() == 3);
    ASSERT_TRUE(j.use_count() == 3);
    ASSERT_TRUE(k.use_count() == 3);
    shared_ptr<std::string> l = k;
    ASSERT_TRUE(i.use_count() == 4);
    ASSERT_TRUE(j.use_count() == 4);
    ASSERT_TRUE(k.use_count() == 4);
    ASSERT_TRUE(l.use_count() == 4);
    ASSERT_TRUE(*i == "3");
    ASSERT_TRUE(*j == "3");
    ASSERT_TRUE(*k == "3");
    ASSERT_TRUE(*l == "3");
    *i = "4";
    ASSERT_TRUE(*i == "4");
    ASSERT_TRUE(*j == "4");
    ASSERT_TRUE(*k == "4");
    ASSERT_TRUE(*l == "4");
    ASSERT_TRUE(i.use_count() == 4);
    ASSERT_TRUE(j.use_count() == 4);
    ASSERT_TRUE(k.use_count() == 4);
    ASSERT_TRUE(l.use_count() == 4);
    }
}

