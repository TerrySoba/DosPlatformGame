#ifndef _TEST_CLASS_H_
#define _TEST_CLASS_H_

#include "shared_ptr.h"

extern int s_instanceCount2;
extern int s_destructorRan2;

struct TestThingy2
{
    TestThingy2(const char* text);
    ~TestThingy2();

    void doIt();

    const char* m_text;
};

tnd::shared_ptr<TestThingy2> createTestThingy2(const char* text);

#endif