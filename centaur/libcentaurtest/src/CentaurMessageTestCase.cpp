#include "CentaurMessageTestCase.h"

#include <bson.h>
#include <bzlib.h>

#include <string.h>

#include <errno.h>

CPPUNIT_TEST_SUITE_REGISTRATION(CentaurMessageTestCase);

void CentaurMessageTestCase::testBSON()
{

	bson::bo objOrginal;
	{
		bson::bob objBuilder;
		objBuilder.append("name", "joe");
		objBuilder.append("age", 33.7);
		objOrginal = objBuilder.obj();
	}

	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> compressedData;
	compressedData.setCount(objOrginal.objsize() * 4);
	unsigned int compressedDataSize = compressedData.getCount();
	CPPUNIT_ASSERT_EQUAL(BZ_OK, BZ2_bzBuffToBuffCompress(compressedData.getData(), &compressedDataSize, (char *)objOrginal.objdata(), objOrginal.objsize(), 9, 0, 30));
	compressedData.setCount(compressedDataSize);

	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> uncompressedData;
	uncompressedData.setCount(compressedData.getCount() * 4);
	unsigned int uncompressedDataSize = uncompressedData.getCount();
	CPPUNIT_ASSERT_EQUAL(BZ_OK, BZ2_bzBuffToBuffDecompress(uncompressedData.getData(), &uncompressedDataSize, (char *)compressedData.getData(), compressedData.getCount(), 0, 0));
	uncompressedData.setCount(uncompressedDataSize);

	CPPUNIT_ASSERT_EQUAL(uncompressedData.getCount(), objOrginal.objsize());

	for (int i = 0; i < uncompressedData.getCount(); i++)
		CPPUNIT_ASSERT_EQUAL(uncompressedData.operator [](i), objOrginal.objdata()[i]);

	bson::bo objUnpacked(uncompressedData.getData());

	CPPUNIT_ASSERT_EQUAL(std::string("joe"), objUnpacked.getField("name").String());
	CPPUNIT_ASSERT_EQUAL(33.7, objUnpacked.getField("age").Number());

}

void CentaurMessageTestCase::testZMQ_ReqRep_helper(CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> &dataOriginal, CentaurSocketRep &rep, CentaurSocketReq &req)
{
	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> dataFromRequest;
	CPPUNIT_ASSERT_EQUAL(-1, rep.recv(dataFromRequest, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, rep.getError());

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.getCount(), req.send(dataOriginal, true));

	for (int tries = 0; tries < 1000; tries++)
	{
		int rc = rep.recv(dataFromRequest, false);
		if (rc >= 0)
		{
			CPPUNIT_ASSERT_EQUAL((int)dataOriginal.getCount(), rc);
			break;
		}
		CPPUNIT_ASSERT_EQUAL(EAGAIN, rep.getError());
		usleep(1);
	}
	CPPUNIT_ASSERT_EQUAL(dataOriginal.getCount(), dataFromRequest.getCount());
	CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.getData(), dataFromRequest.getData(), dataOriginal.getSize()));


	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> dataFromResponse;
	CPPUNIT_ASSERT_EQUAL(-1, req.recv(dataFromResponse, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, req.getError());

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.getCount(), rep.send(dataOriginal, true));

	for (int tries = 0; tries < 1000; tries++)
	{
		int rc = req.recv(dataFromResponse, false);
		if (rc >= 0)
		{
			CPPUNIT_ASSERT_EQUAL((int)dataOriginal.getCount(), rc);
			break;
		}
		CPPUNIT_ASSERT_EQUAL(EAGAIN, req.getError());
		usleep(1);
	}
	CPPUNIT_ASSERT_EQUAL(dataOriginal.getCount(), dataFromResponse.getCount());
	CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.getData(), dataFromResponse.getData(), dataOriginal.getSize()));
}

void CentaurMessageTestCase::testZMQ_ReqRep()
{
	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> dataOriginal;
	dataOriginal.setCount(ZEROMQ_DATA_FACTOR * 256);

	for (int i = 0; i < ZEROMQ_DATA_FACTOR; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			int index = (i * 256) + j;
			CPPUNIT_ASSERT(index < (int)dataOriginal.getCount());
			dataOriginal.operator [](index) = j;
		}
	}

	CentaurSocketRep rep;
	CPPUNIT_ASSERT(rep.open("tcp://127.0.0.1:5560"));
	CPPUNIT_ASSERT(rep.bind("ipc://ReqRepTest_1.ipc"));

	{
		CentaurSocketReq req;
		CPPUNIT_ASSERT(req.open("tcp://127.0.0.1:5560"));

		testZMQ_ReqRep_helper(dataOriginal, rep, req);
	}

	{
		CentaurSocketReq req;
		CPPUNIT_ASSERT(req.open("ipc://ReqRepTest_1.ipc"));

		testZMQ_ReqRep_helper(dataOriginal, rep, req);
	}
}

void CentaurMessageTestCase::testZMQ_ReqRep_10000_helper(const char * addr)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	double t1 = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; // convert tv_sec & tv_usec to millisecond

	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> dataOriginal;
	dataOriginal.setCount(ZEROMQ_DATA_FACTOR * 256);

	for (int i = 0; i < ZEROMQ_DATA_FACTOR; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			int index = (i * 256) + j;
			CPPUNIT_ASSERT(index < (int)dataOriginal.getCount());
			dataOriginal.operator [](index) = j;
		}
	}

	CentaurSocketRep rep;
	CPPUNIT_ASSERT(rep.open(addr));

	CentaurSocketReq req;
	CPPUNIT_ASSERT(req.open(addr));

	for (int i = 0; i < 10000; i++)
	{
		if ((i % 1000) == 0 && i != 0)
		{
			struct timeval tv;
			gettimeofday(&tv, NULL);

			double diff = ((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000) - t1; // convert tv_sec & tv_usec to millisecond

			std::cerr << i << " in " << diff << " milliseconds (" << diff / (double)i << " milliseconds per bounce) of " << dataOriginal.getCount() / 1024 << "KB" << "\n";
		}
		testZMQ_ReqRep_helper(dataOriginal, rep, req);
	}
}

void CentaurMessageTestCase::testZMQ_ReqRep_10000_IPC()
{
	return testZMQ_ReqRep_10000_helper("ipc://ReqRepTest_2.ipc");
}

void CentaurMessageTestCase::testZMQ_ReqRep_10000_TCP()
{
	return testZMQ_ReqRep_10000_helper("tcp://127.0.0.1:5561");
}

void CentaurMessageTestCase::testZMQ_PubSub()
{
	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> dataOriginal;
	dataOriginal.setCount(ZEROMQ_DATA_FACTOR * 256);

	for (int i = 0; i < ZEROMQ_DATA_FACTOR; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			int index = (i * 256) + j;
			CPPUNIT_ASSERT(index < (int)dataOriginal.getCount());
			dataOriginal.operator [](index) = j;
		}
	}

	CentaurSocketPub pub;
	CPPUNIT_ASSERT(pub.open("tcp://127.0.0.1:5562"));

	CentaurSocketSub sub;
	CPPUNIT_ASSERT(sub.open("tcp://127.0.0.1:5562", false));

	CM_Array<char, 256 * ZEROMQ_DATA_FACTOR> dataFromSubscriber;
	CPPUNIT_ASSERT_EQUAL(-1, sub.recv(dataFromSubscriber, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, sub.getError());

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.getCount(), pub.send(dataOriginal, true));

	CPPUNIT_ASSERT_EQUAL(-1, sub.recv(dataFromSubscriber, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, sub.getError());

	const char * filter = "bob";
	CPPUNIT_ASSERT_EQUAL(0, sub.subscribe(filter, strlen(filter)));

	sleep(1); // give the socket the chance to apply the filter...

	memcpy(dataOriginal.getData(), filter, strlen(filter));

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.getCount(), pub.send(dataOriginal, true));

	for (int tries = 0; tries < 1000; tries++)
	{
		int rc = sub.recv(dataFromSubscriber, false);
		if (rc >= 0)
		{
			CPPUNIT_ASSERT_EQUAL((int)dataOriginal.getCount(), rc);
			break;
		}
		CPPUNIT_ASSERT_EQUAL(EAGAIN, sub.getError());
		usleep(1);
	}
	CPPUNIT_ASSERT_EQUAL(dataOriginal.getCount(), dataFromSubscriber.getCount());
	CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.getData(), dataFromSubscriber.getData(), dataOriginal.getSize()));

}
