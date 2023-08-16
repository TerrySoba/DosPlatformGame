#include "test_class.h"

#include <stdio.h>

int s_instanceCount2 = 0;
int s_destructorRan2 = false;

TestThingy2::TestThingy2(const char *text) : m_text(text)
{
    ++s_instanceCount2;
}

TestThingy2::~TestThingy2()
{
    printf("~TestThingy2: %s\n", m_text);
    s_destructorRan2 = true;
    --s_instanceCount2;
}

void TestThingy2::doIt()
{
    printf("doIt: %s\n", m_text);
}


tnd::shared_ptr<TestThingy2> createTestThingy2(const char* text)
{
    return tnd::shared_ptr<TestThingy2>(new TestThingy2(text));
}