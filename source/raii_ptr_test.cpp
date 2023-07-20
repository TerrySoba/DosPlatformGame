#include "unit_test.h"
#include "raii_ptr.h"


static bool s_raiiDestructorRan = false;

struct RaiiTestThingy
{
    RaiiTestThingy(const char* text) :
        m_text(text)
    {
    }

    ~RaiiTestThingy()
    {
        s_raiiDestructorRan = true;
    }

    void doIt() {
    }

    const char* m_text;
};

TEST(RaiiPtrTestDestructor)
{
    s_raiiDestructorRan = false;

    {
        RaiiTestThingy* thingy = new RaiiTestThingy("test1");
        raii_ptr<RaiiTestThingy> thingyPtr(thingy);
        ASSERT_FALSE(s_raiiDestructorRan);
        ASSERT_TRUE(thingyPtr.get() == thingy);
    }
    ASSERT_TRUE(s_raiiDestructorRan);
}

TEST(RaiiPtrTestNullptr)
{
    raii_ptr<RaiiTestThingy> thingyPtr(0);
    ASSERT_TRUE(thingyPtr.get() == 0);
}

