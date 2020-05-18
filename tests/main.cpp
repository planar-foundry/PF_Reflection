#include <PF_Test/UnitTest.hpp>
#include <PF_Debug/Assert.hpp>
#include <PF_Debug/Log.hpp>

void pf::test::unit_test_init(UnitTestResult** result)
{
    static UnitTestResult** s_current_test = result;

    pf::debug::set_assert_handler(
        [](const char* cond, const char* file, int line, const char* msg)
    {
        if (!(*s_current_test)->ignore_asserts)
        {
            printf(" Failed assert %s at %s:%d %s ", cond, file, line, msg);
            fflush(stdout);
            (*s_current_test)->failed_assert = true;
            debug_break();
        }
    });

    pf::debug::set_log_handler(
        [](pf::debug::Severity sev, const char* msg)
    {
        if (!(*s_current_test)->ignore_log)
        {
            printf(" %d %s ", (int)sev, msg);
            fflush(stdout);
            if (sev == pf::debug::Error || sev == pf::debug::Warn)
            {
                (*s_current_test)->failed_log = true;
                debug_break();
            }
        }
    });
}

void pf::test::unit_test_free() { }
