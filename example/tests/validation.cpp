#include "dbus.h"

using namespace DBus::Validation;

#define TEST_QUIET_FAIL_ONLY 1

void testValidation()
{

    auto should_pass = [](std::function<void(const std::string&)> fn, const std::string& param) {
        try {
            fn(param);
#if TEST_QUIET_FAIL_ONLY == 0
            printf("Testing : %s : PASS\n", param.c_str());
#endif
        } catch (const std::exception& e) {
            printf("Testing : %s : FAIL : exception thrown (%s)\n", param.c_str(), e.what());
        }
    };

    should_pass(throwOnInvalidSignature, "");
    should_pass(throwOnInvalidSignature, "ai");

    should_pass(throwOnInvalidObjectPath, "/");
    should_pass(throwOnInvalidObjectPath, "/biz/brightsign/fake_name");

    should_pass(throwOnInvalidBasicType, "y");
    should_pass(throwOnInvalidBasicType, "b");
    should_pass(throwOnInvalidBasicType, "n");
    should_pass(throwOnInvalidBasicType, "q");
    should_pass(throwOnInvalidBasicType, "i");
    should_pass(throwOnInvalidBasicType, "u");
    should_pass(throwOnInvalidBasicType, "x");
    should_pass(throwOnInvalidBasicType, "t");
    should_pass(throwOnInvalidBasicType, "d");
    should_pass(throwOnInvalidBasicType, "h");
    should_pass(throwOnInvalidSingleCompleteType, "(ii)");
    should_pass(throwOnInvalidSingleCompleteType, "a{ii}");
    should_pass(throwOnInvalidSingleCompleteType, "a{ia(y)}");

    should_pass(throwOnInvalidInterfaceName, "biz.brightsign.is_valid");

    should_pass(throwOnInvalidBusName, ":1.12");
    should_pass(throwOnInvalidBusName, "bus.name");

    should_pass(throwOnInvalidMethodName, "name0");

    // Next we test a series of case which are invalid, and should throw.
    auto should_fail = [](std::function<void(const std::string&)> fn, const std::string& param) {
        try {
            fn(param);
        } catch (...) {
#if TEST_QUIET_FAIL_ONLY == 0
            printf("Testing : %s : PASS\n", param.c_str());
#endif
            return;
        }
        printf("Testing : %s : FAIL : nothing throw\n", param.c_str());
    };

    should_fail(throwOnInvalidObjectPath, "/biz/brightsign/fake_name/");

    should_fail(throwOnInvalidBasicType, "ai");
    should_fail(throwOnInvalidBasicType, "yv");
    should_fail(throwOnInvalidBasicType, "a(i)");

    should_fail(throwOnInvalidSingleCompleteType, "ii");
    should_fail(throwOnInvalidSingleCompleteType, "aiai");
    should_fail(throwOnInvalidSingleCompleteType, "(ii)(ii)");

    should_fail(throwOnInvalidInterfaceName, "biz");
    should_fail(throwOnInvalidInterfaceName, "biz.brightsign.is-invalid");

    should_fail(throwOnInvalidBusName, "123*2");
    should_fail(throwOnInvalidBusName, "123");
    should_fail(throwOnInvalidBusName, "123.456");
    should_fail(throwOnInvalidBusName, ".123.456");

    should_fail(throwOnInvalidMethodName, "my.name");
    should_fail(throwOnInvalidMethodName, "my-name");
    should_fail(throwOnInvalidMethodName, "");
}

int main(int argc, const char* argv[])
{
    printf("Tests start...\n");
    testValidation();
    printf("End.\n");
}
