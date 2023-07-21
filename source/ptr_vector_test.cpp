#include "unit_test.h"
#include "ptr_vector.h"

using namespace tnd;

TEST(PtrVectorTest)
{
    ptr_vector<int> vec;

    int* a = new int(3);
    int* b = new int(4);
    int* c = new int(7);

    vec.push_back(a);
    vec.push_back(b);
    vec.push_back(c);
    ASSERT_TRUE(vec.size() == 3);

    ASSERT_TRUE(*vec[0] == 3);
    ASSERT_TRUE(*vec[1] == 4);
    ASSERT_TRUE(*vec[2] == 7);

    ASSERT_TRUE(vec[0] == a);
    ASSERT_TRUE(vec[1] == b);
    ASSERT_TRUE(vec[2] == c);
}

static bool s_destructorCalled = false;

class PtrVectorTestClass
{
public:
    ~PtrVectorTestClass()
    {
        s_destructorCalled = true;
    }
};


TEST(PtrVectorDestructorTest)
{
    s_destructorCalled = false;
    {
        ptr_vector<PtrVectorTestClass> vec;
        vec.push_back(new PtrVectorTestClass());
        ASSERT_TRUE(vec.size() == 1);
        ASSERT_FALSE(s_destructorCalled);
    }
    ASSERT_TRUE(s_destructorCalled);
}
