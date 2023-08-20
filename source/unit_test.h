#ifndef UNIT_TEST_H_INCLUDED
#define UNIT_TEST_H_INCLUDED

#include <map>
#include <string>
#include <iostream>
#include <stdio.h>

enum TestResult
{
	TEST_FAILURE = 0,
	TEST_SUCCESS = 1
};

typedef void (*TestFunctionPtr)();

extern TestResult s_currentTestResult;

std::map<std::string, TestFunctionPtr>& getTests();

const size_t s_lastMessageBufferSize = 128;
extern char s_lastMessageBuffer[s_lastMessageBufferSize];

// returns true on success, false if at least one test failed
bool runTests(const char* filter);

#define _xstr(s) _ystr(s)
#define _ystr(s) #s

#define RUN_STATIC(expr, line) \
	class StaticRunner ## line \
	{ \
	public: \
		StaticRunner ## line () { expr; } \
	}; \
	static StaticRunner ## line staticRunner ## line;

#define TEST(name) \
	void test_ ## name (); \
	RUN_STATIC(getTests()[_xstr(name)] = &test_ ## name, name); \
	void test_ ## name ()

#define ASSERT_TRUE(expr) \
	if (!(expr)) \
	{ \
		snprintf(s_lastMessageBuffer, s_lastMessageBufferSize, "Assert failed: ASSERT_TRUE("  _xstr(expr)  ") " __FILE__ ":%d", __LINE__); \
		std::cout << s_lastMessageBuffer << "\n"; \
		s_currentTestResult = TEST_FAILURE; \
		return; \
	}

#define ASSERT_THROW(expr, expected_exception) \
	try { \
		expr; \
		snprintf(s_lastMessageBuffer, s_lastMessageBufferSize, "Assert failed: ASSERT_THROW("  _xstr(expr)  ") but actually it did not throw. " __FILE__ ":%d", __LINE__); \
		std::cout << s_lastMessageBuffer << "\n"; \
		s_currentTestResult = TEST_FAILURE; \
		return; \
	} catch (const expected_exception&)	{}

#define ASSERT_FALSE(expr) \
	if ((expr)) \
	{ \
		snprintf(s_lastMessageBuffer, s_lastMessageBufferSize, "Assert failed: ASSERT_FALSE("  _xstr(expr)  ") " __FILE__ ":%d", __LINE__); \
		std::cout << s_lastMessageBuffer << "\n"; \
		s_currentTestResult = TEST_FAILURE; \
		return; \
	}

#endif
