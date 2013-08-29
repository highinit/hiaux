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
bool suite_HdividerDistTests_init = false;
#include "/Users/phrk/Projects/highinit_repos/tetramorph/hsharder/tests/hsharder_test.h"

static HdividerDistTests suite_HdividerDistTests;

static CxxTest::List Tests_HdividerDistTests = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_HdividerDistTests( "hsharder_test.h", 12, "HdividerDistTests", suite_HdividerDistTests, Tests_HdividerDistTests );

static class TestDescription_suite_HdividerDistTests_testO : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerDistTests_testO() : CxxTest::RealTestDescription( Tests_HdividerDistTests, suiteDescription_HdividerDistTests, 16, "testO" ) {}
 void runTest() { suite_HdividerDistTests.testO(); }
} testDescription_suite_HdividerDistTests_testO;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
