#include "unit_test.h"
#include "vector.h"

TEST(VectorTest1)
{
    tnd::vector<int> vec1;
    vec1.push_back(3);
    vec1.push_back(4);
    vec1.push_back(7);
    ASSERT_TRUE(vec1.size() == 3);

    tnd::vector<int> vec2(vec1);
    ASSERT_TRUE(vec1[0] == vec2[0]);
    ASSERT_TRUE(vec1[1] == vec2[1]);
    ASSERT_TRUE(vec1[2] == vec2[2]);
    ASSERT_TRUE(vec2.size() == 3);

    tnd::vector<int> vec3;
    vec3 = vec1;
    ASSERT_TRUE(vec1[0] == vec3[0]);
    ASSERT_TRUE(vec1[1] == vec3[1]);
    ASSERT_TRUE(vec1[2] == vec3[2]);
    ASSERT_TRUE(vec3.size() == 3);
}

TEST(VectorTestErase)
{
    tnd::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    ASSERT_TRUE(vec.size() == 4);
    ASSERT_TRUE(vec[0] == 1);
    ASSERT_TRUE(vec[1] == 2);
    ASSERT_TRUE(vec[2] == 3);
    ASSERT_TRUE(vec[3] == 4);

    vec.erase(1);

    ASSERT_TRUE(vec.size() == 3);
    ASSERT_TRUE(vec[0] == 1);
    ASSERT_TRUE(vec[1] == 3);
    ASSERT_TRUE(vec[2] == 4);
}


TEST(VectorResizeTest)
{
    tnd::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    vec.resize(2);
    ASSERT_TRUE(vec.size() == 2);
    ASSERT_TRUE(vec[0] == 1);
    ASSERT_TRUE(vec[1] == 2);


    vec.resize(10);
    ASSERT_TRUE(vec.size() == 10);
    ASSERT_TRUE(vec[0] == 1);
    ASSERT_TRUE(vec[1] == 2);
    // elements 2, 3, 4 are undefined

}

TEST(VectorAssignReusesBuffer)
{
    tnd::vector<int> big;
    big.push_back(1);
    big.push_back(2);
    big.push_back(3);
    tnd::vector<int> small;
    small.push_back(9);

    tnd::vector<int> vec;
    vec.assign(big);
    ASSERT_TRUE(vec.size() == 3);
    ASSERT_TRUE(vec[0] == 1 && vec[1] == 2 && vec[2] == 3);

    // a smaller vector must not cause a reallocation
    int* buffer = vec.data();
    vec.assign(small);
    ASSERT_TRUE(vec.size() == 1);
    ASSERT_TRUE(vec[0] == 9);
    ASSERT_TRUE(vec.data() == buffer);

    // growing again within the capacity must not reallocate either
    vec.assign(big);
    ASSERT_TRUE(vec.size() == 3);
    ASSERT_TRUE(vec[0] == 1 && vec[1] == 2 && vec[2] == 3);
    ASSERT_TRUE(vec.data() == buffer);

    vec.assign(vec);
    ASSERT_TRUE(vec.size() == 3);
    ASSERT_TRUE(vec[2] == 3);
}
