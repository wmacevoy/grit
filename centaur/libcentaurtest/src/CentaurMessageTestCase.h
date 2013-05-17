#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include <CentaurSockets.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class CentaurMessageTestCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CentaurMessageTestCase);
	CPPUNIT_TEST(testBSON);
	CPPUNIT_TEST(testZMQ_ReqRep);
	CPPUNIT_TEST(testZMQ_ReqRep_10000);
	CPPUNIT_TEST(testZMQ_PubSub);
	CPPUNIT_TEST_SUITE_END();

protected:

public:

protected:
	void testBSON();
	void testZMQ_ReqRep_helper(std::vector<char> &dataOriginal, CentaurSocketRep &rep, CentaurSocketReq &req);
	void testZMQ_ReqRep();
	void testZMQ_ReqRep_10000();
	void testZMQ_PubSub();
};
