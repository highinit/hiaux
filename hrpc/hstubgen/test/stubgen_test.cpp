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
bool suite_fault_test_suite_init = false;
#include "/Users/phrk/Projects/meroving/hstubgen/test/fault_test.h"

static fault_test_suite suite_fault_test_suite;

static CxxTest::List Tests_fault_test_suite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_fault_test_suite( "fault_test.h", 2, "fault_test_suite", suite_fault_test_suite, Tests_fault_test_suite );

static class TestDescription_suite_fault_test_suite_testFault : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fault_test_suite_testFault() : CxxTest::RealTestDescription( Tests_fault_test_suite, suiteDescription_fault_test_suite, 11, "testFault" ) {}
 void runTest() { suite_fault_test_suite.testFault(); }
} testDescription_suite_fault_test_suite_testFault;

#include "/Users/phrk/Projects/meroving/hstubgen/test/stubgen_test.h"

static stubgen_test suite_stubgen_test;

static CxxTest::List Tests_stubgen_test = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_stubgen_test( "stubgen_test.h", 7, "stubgen_test", suite_stubgen_test, Tests_stubgen_test );

static class TestDescription_suite_stubgen_test_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_stubgen_test_test1() : CxxTest::RealTestDescription( Tests_stubgen_test, suiteDescription_stubgen_test, 20, "test1" ) {}
 void runTest() { suite_stubgen_test.test1(); }
} testDescription_suite_stubgen_test_test1;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
