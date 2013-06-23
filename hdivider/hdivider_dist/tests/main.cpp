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
bool suite_HdividerDistTests_init = false;
#include "/Users/phrk/Projects/highinit_repos/tetramorph/hdivider/hdivider_dist/tests/hdivider_dist_tests.h"

static HdividerDistTests suite_HdividerDistTests;

static CxxTest::List Tests_HdividerDistTests = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_HdividerDistTests( "hdivider_dist_tests.h", 37, "HdividerDistTests", suite_HdividerDistTests, Tests_HdividerDistTests );

static class TestDescription_suite_HdividerDistTests_testOneWorker : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerDistTests_testOneWorker() : CxxTest::RealTestDescription( Tests_HdividerDistTests, suiteDescription_HdividerDistTests, 46, "testOneWorker" ) {}
 void runTest() { suite_HdividerDistTests.testOneWorker(); }
} testDescription_suite_HdividerDistTests_testOneWorker;

static class TestDescription_suite_HdividerDistTests_testConcurrentWriteResult : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerDistTests_testConcurrentWriteResult() : CxxTest::RealTestDescription( Tests_HdividerDistTests, suiteDescription_HdividerDistTests, 104, "testConcurrentWriteResult" ) {}
 void runTest() { suite_HdividerDistTests.testConcurrentWriteResult(); }
} testDescription_suite_HdividerDistTests_testConcurrentWriteResult;

static class TestDescription_suite_HdividerDistTests_testConcurrentWriteResultusingDb : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerDistTests_testConcurrentWriteResultusingDb() : CxxTest::RealTestDescription( Tests_HdividerDistTests, suiteDescription_HdividerDistTests, 221, "testConcurrentWriteResultusingDb" ) {}
 void runTest() { suite_HdividerDistTests.testConcurrentWriteResultusingDb(); }
} testDescription_suite_HdividerDistTests_testConcurrentWriteResultusingDb;

static class TestDescription_suite_HdividerDistTests_testConcurrentWriteResultUsingDbUsingHdividerCache : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerDistTests_testConcurrentWriteResultUsingDbUsingHdividerCache() : CxxTest::RealTestDescription( Tests_HdividerDistTests, suiteDescription_HdividerDistTests, 344, "testConcurrentWriteResultUsingDbUsingHdividerCache" ) {}
 void runTest() { suite_HdividerDistTests.testConcurrentWriteResultUsingDbUsingHdividerCache(); }
} testDescription_suite_HdividerDistTests_testConcurrentWriteResultUsingDbUsingHdividerCache;

static class TestDescription_suite_HdividerDistTests_testMongoIdAccessor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerDistTests_testMongoIdAccessor() : CxxTest::RealTestDescription( Tests_HdividerDistTests, suiteDescription_HdividerDistTests, 465, "testMongoIdAccessor" ) {}
 void runTest() { suite_HdividerDistTests.testMongoIdAccessor(); }
} testDescription_suite_HdividerDistTests_testMongoIdAccessor;

static class TestDescription_suite_HdividerDistTests_testMongoStateAccessor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerDistTests_testMongoStateAccessor() : CxxTest::RealTestDescription( Tests_HdividerDistTests, suiteDescription_HdividerDistTests, 506, "testMongoStateAccessor" ) {}
 void runTest() { suite_HdividerDistTests.testMongoStateAccessor(); }
} testDescription_suite_HdividerDistTests_testMongoStateAccessor;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
