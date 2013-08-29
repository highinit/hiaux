/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_hPoolServerTests_init = false;
#include "/Users/phrk/Projects/highinit_repos/tetramorph/hpoolserver/tests/hpoolserver_tests.h"

static hPoolServerTests suite_hPoolServerTests;

static CxxTest::List Tests_hPoolServerTests = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_hPoolServerTests( "hpoolserver_tests.h", 6, "hPoolServerTests", suite_hPoolServerTests, Tests_hPoolServerTests );

static class TestDescription_suite_hPoolServerTests_testO : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_hPoolServerTests_testO() : CxxTest::RealTestDescription( Tests_hPoolServerTests, suiteDescription_hPoolServerTests, 11, "testO" ) {}
 void runTest() { suite_hPoolServerTests.testO(); }
} testDescription_suite_hPoolServerTests_testO;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
