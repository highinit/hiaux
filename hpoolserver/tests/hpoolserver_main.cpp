/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
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
CxxTest::StaticSuiteDescription suiteDescription_hPoolServerTests( "hpoolserver_tests.h", 142, "hPoolServerTests", suite_hPoolServerTests, Tests_hPoolServerTests );

static class TestDescription_suite_hPoolServerTests_testCheckInitConnection : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_hPoolServerTests_testCheckInitConnection() : CxxTest::RealTestDescription( Tests_hPoolServerTests, suiteDescription_hPoolServerTests, 175, "testCheckInitConnection" ) {}
 void runTest() { suite_hPoolServerTests.testCheckInitConnection(); }
} testDescription_suite_hPoolServerTests_testCheckInitConnection;

static class TestDescription_suite_hPoolServerTests_testSendSimple : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_hPoolServerTests_testSendSimple() : CxxTest::RealTestDescription( Tests_hPoolServerTests, suiteDescription_hPoolServerTests, 205, "testSendSimple" ) {}
 void runTest() { suite_hPoolServerTests.testSendSimple(); }
} testDescription_suite_hPoolServerTests_testSendSimple;

static class TestDescription_suite_hPoolServerTests_testPoolServer : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_hPoolServerTests_testPoolServer() : CxxTest::RealTestDescription( Tests_hPoolServerTests, suiteDescription_hPoolServerTests, 294, "testPoolServer" ) {}
 void runTest() { suite_hPoolServerTests.testPoolServer(); }
} testDescription_suite_hPoolServerTests_testPoolServer;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
