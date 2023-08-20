#include "unit_test.h"

#include "exception.h"

#include <string.h>
#include <iostream>
#include <strstrea>

char s_lastMessageBuffer[s_lastMessageBufferSize];

std::map<std::string, TestFunctionPtr>& getTests()
{
    static std::map<std::string, TestFunctionPtr> tests;
    return tests;
}

TestResult s_currentTestResult;

bool runTests(const char* filter)
{
    FILE* junit = fopen("junit.xml", "w");
    fprintf(junit, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(junit, "<testsuites>\n");
    fprintf(junit, "  <testsuite name=\"AllTests\">\n");
    std::cout << "Running " << getTests().size() << " tests." << std::endl;
    if (filter != NULL) {
        std::cout << "Using filter: " << filter << "\n";
    }
    int i = 1;
    int failureCount = 0;
    for (std::map<std::string, TestFunctionPtr>::iterator it = getTests().begin(); it != getTests().end(); ++it)
    {
        std::string name = (it->first);
        
        if (
            ((filter == NULL) || strstr(name.c_str(), filter)) &&
            (!strstr(name.c_str(), "DISABLED_"))
            )
        {
            fprintf(junit, "    <testcase name=\"%s\">\n", name.c_str());
            std::cout << "[ RUN       ] " << name.c_str() << " (" << i++ << " of " << getTests().size() << ")" << std::endl;
            s_currentTestResult = TEST_SUCCESS;
            try
            {
                it->second();
            }
            catch(const Exception& e)
            {
                snprintf(s_lastMessageBuffer, s_lastMessageBufferSize, "Exception: %s", e.what());
                std::cout << s_lastMessageBuffer << "\n";
                s_currentTestResult = TEST_FAILURE;
            }
            catch(const std::exception& e)
            {
                snprintf(s_lastMessageBuffer, s_lastMessageBufferSize, "std::exception: %s", e.what());
                std::cout << s_lastMessageBuffer << "\n";
                s_currentTestResult = TEST_FAILURE;
            }
            catch(...)
            {
                snprintf(s_lastMessageBuffer, s_lastMessageBufferSize, "Unknown exception");
                std::cout << s_lastMessageBuffer << "\n";
                s_currentTestResult = TEST_FAILURE;
            }
            
            if (s_currentTestResult != TEST_SUCCESS)
            {
                ++failureCount;
                fprintf(junit, "      <failure message=\"%s\"></failure>\n", s_lastMessageBuffer);
            }
            std::cout << "[      " << ((s_currentTestResult == TEST_SUCCESS)?"  OK":"FAIL") << " ] " << name.c_str() << std::endl;

            fprintf(junit, "    </testcase>\n");

            if (failureCount > 0) break;
        }
    }

    fprintf(junit, "  </testsuite>\n");
    fprintf(junit, "</testsuites>\n");
    fclose(junit);
    std::cout << "Failed tests: " << failureCount << std::endl;
    return (failureCount > 0);
}
