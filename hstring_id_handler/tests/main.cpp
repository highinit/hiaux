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
bool suite_HwordTests_init = false;
#include "/Users/phrk/Projects/highinit_repos/hword_id_handler/tests/hword_tests.h"

static HwordTests suite_HwordTests;

static CxxTest::List Tests_HwordTests = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_HwordTests( "hword_tests.h", 36, "HwordTests", suite_HwordTests, Tests_HwordTests );

static class TestDescription_suite_HwordTests_testSimple : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HwordTests_testSimple() : CxxTest::RealTestDescription( Tests_HwordTests, suiteDescription_HwordTests, 74, "testSimple" ) {}
 void runTest() { suite_HwordTests.testSimple(); }
} testDescription_suite_HwordTests_testSimple;

static class TestDescription_suite_HwordTests_testDistributed : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HwordTests_testDistributed() : CxxTest::RealTestDescription( Tests_HwordTests, suiteDescription_HwordTests, 102, "testDistributed" ) {}
 void runTest() { suite_HwordTests.testDistributed(); }
} testDescription_suite_HwordTests_testDistributed;

static class TestDescription_suite_HwordTests_testMongoDbAccessor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HwordTests_testMongoDbAccessor() : CxxTest::RealTestDescription( Tests_HwordTests, suiteDescription_HwordTests, 144, "testMongoDbAccessor" ) {}
 void runTest() { suite_HwordTests.testMongoDbAccessor(); }
} testDescription_suite_HwordTests_testMongoDbAccessor;

static class TestDescription_suite_HwordTests_testDistributedWithDb : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HwordTests_testDistributedWithDb() : CxxTest::RealTestDescription( Tests_HwordTests, suiteDescription_HwordTests, 182, "testDistributedWithDb" ) {}
 void runTest() { suite_HwordTests.testDistributedWithDb(); }
} testDescription_suite_HwordTests_testDistributedWithDb;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
