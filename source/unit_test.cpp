#include "unit_test.h"

#include "exception.h"

#include <iostream>

std::map<std::string, TestFunctionPtr>& getTests()
{
    static std::map<std::string, TestFunctionPtr> tests;
    return tests;
}

TestResult s_currentTestResult;

bool runTests()
{
    FILE* junit = NULL;
    std::cout << "Running " << getTests().size() << " tests." << std::endl;
    int i = 1;
    int failureCount = 0;
    for (std::map<std::string, TestFunctionPtr>::iterator it = getTests().begin(); it != getTests().end(); ++it)
    {
        std::string name = (it->first);
        std::cout << "[ RUN       ] " << name.c_str() << " (" << i++ << " of " << getTests().size() << ")" << std::endl;
        s_currentTestResult = TEST_SUCCESS;
        try
        {
            it->second();
        }
        catch(const Exception& e)
        {
            std::cout << "Unexpected exception:" << e.what() << '\n';
            s_currentTestResult = TEST_FAILURE;
        }
        catch(const std::exception& e)
        {
            std::cout << "Unexpected exception:" << e.what() << '\n';
            s_currentTestResult = TEST_FAILURE;
        }
        catch(...)
        {
            std::cout << "Unknown exception\n";
            s_currentTestResult = TEST_FAILURE;
        }
        
        if (s_currentTestResult != TEST_SUCCESS)
        {
            ++failureCount;
        }
        std::cout << "[      " << ((s_currentTestResult == TEST_SUCCESS)?"  OK":"FAIL") << " ] " << name.c_str() << std::endl;


        if (failureCount > 0) break;
    }

    std::cout << "Failed tests: " << failureCount << std::endl;
    return (failureCount > 0);
}
