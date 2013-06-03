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
bool suite_HdividerTests_init = false;
#include "/Users/phrk/Projects/highinit_repos/hdivider/tests/hdivider_test.h"

static HdividerTests suite_HdividerTests;

static CxxTest::List Tests_HdividerTests = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_HdividerTests( "hdivider_test.h", 209, "HdividerTests", suite_HdividerTests, Tests_HdividerTests );

static class TestDescription_suite_HdividerTests_testOneWorker : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerTests_testOneWorker() : CxxTest::RealTestDescription( Tests_HdividerTests, suiteDescription_HdividerTests, 220, "testOneWorker" ) {}
 void runTest() { suite_HdividerTests.testOneWorker(); }
} testDescription_suite_HdividerTests_testOneWorker;

static class TestDescription_suite_HdividerTests_testConcurrentReadInput : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerTests_testConcurrentReadInput() : CxxTest::RealTestDescription( Tests_HdividerTests, suiteDescription_HdividerTests, 222, "testConcurrentReadInput" ) {}
 void runTest() { suite_HdividerTests.testConcurrentReadInput(); }
} testDescription_suite_HdividerTests_testConcurrentReadInput;

static class TestDescription_suite_HdividerTests_testConcurrentWriteResult : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerTests_testConcurrentWriteResult() : CxxTest::RealTestDescription( Tests_HdividerTests, suiteDescription_HdividerTests, 224, "testConcurrentWriteResult" ) {}
 void runTest() { suite_HdividerTests.testConcurrentWriteResult(); }
} testDescription_suite_HdividerTests_testConcurrentWriteResult;

static class TestDescription_suite_HdividerTests_testOneFault : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_HdividerTests_testOneFault() : CxxTest::RealTestDescription( Tests_HdividerTests, suiteDescription_HdividerTests, 226, "testOneFault" ) {}
 void runTest() { suite_HdividerTests.testOneFault(); }
} testDescription_suite_HdividerTests_testOneFault;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
