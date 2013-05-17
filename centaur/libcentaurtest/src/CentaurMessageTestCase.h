#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include <CentaurSockets.h>

#define ZEROMQ_DATA_FACTOR		4 // 1KB
//#define ZEROMQ_DATA_FACTOR		4096 // 1MB

class CentaurMessageTestCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CentaurMessageTestCase);
	CPPUNIT_TEST(testBSON);
	CPPUNIT_TEST(testZMQ_ReqRep);
	CPPUNIT_TEST(testZMQ_ReqRep_10000_IPC);
	CPPUNIT_TEST(testZMQ_ReqRep_10000_TCP);
	CPPUNIT_TEST(testZMQ_PubSub);
	CPPUNIT_TEST_SUITE_END();

protected:

public:

protected:
	void testBSON();
	void testZMQ_ReqRep_helper(CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> &dataOriginal, CentaurSocketRep &rep, CentaurSocketReq &req);
	void testZMQ_ReqRep();
	void testZMQ_ReqRep_10000_helper(const char * addr);
	void testZMQ_ReqRep_10000_IPC();
	void testZMQ_ReqRep_10000_TCP();
	void testZMQ_PubSub();
};
