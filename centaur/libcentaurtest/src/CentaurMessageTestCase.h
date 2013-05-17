#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include <CentaurSockets.h>

#define ZEROMQ_DATA_FACTOR		4096

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
	void testZMQ_ReqRep_helper(CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> &dataOriginal, CentaurSocketRep &rep, CentaurSocketReq &req);
	void testZMQ_ReqRep();
	void testZMQ_ReqRep_10000();
	void testZMQ_PubSub();
};
