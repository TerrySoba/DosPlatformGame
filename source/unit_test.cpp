#include "unit_test.h"

#include "exception.h"

#include <string.h>
#include <iostream>


#ifdef __GNUC__
#include <sstream>
#else
#include <strstrea>
#endif


char s_lastMessageBuffer[s_lastMessageBufferSize];

std::map<std::string, TestFunctionPtr>& getTests()
{
    static std::map<std::string, TestFunctionPtr> tests;
    return tests;
}

TestResult s_currentTestResult;


void quoteXmlString(char* buffer, size_t bufferLength)
{
    size_t pos = 0;
    while (buffer[pos] != 0)
    {
        if (buffer[pos] == '<') {
            memmove(buffer + pos + 4, buffer + pos + 1, bufferLength - pos - 4);
            memcpy(buffer + pos, "&lt;", 4);
            pos += 4;
        }
        else if (buffer[pos] == '>') {
            memmove(buffer + pos + 4, buffer + pos + 1, bufferLength - pos - 4);
            memcpy(buffer + pos, "&gt;", 4);
            pos += 4;
        }
        else if (buffer[pos] == '&') {
            memmove(buffer + pos + 5, buffer + pos + 1, bufferLength - pos - 5);
            memcpy(buffer + pos, "&amp;", 5);
            pos += 5;
        }
        else if (buffer[pos] == '"') {
            memmove(buffer + pos + 6, buffer + pos + 1, bufferLength - pos - 6);
            memcpy(buffer + pos, "&quot;", 6);
            pos += 6;
        }
        else if (buffer[pos] == '\'') {
            memmove(buffer + pos + 6, buffer + pos + 1, bufferLength - pos - 6);
            memcpy(buffer + pos, "&apos;", 6);
            pos += 6;
        }
        else {
            ++pos;
        }
    }
}

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
                quoteXmlString(s_lastMessageBuffer, s_lastMessageBufferSize);
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
