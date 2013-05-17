#include "CentaurMessageTestCase.h"

#include <bson/bson.h>
#include <bzip2/bzlib.h>

#include <string.h>

#include <errno.h>

CPPUNIT_TEST_SUITE_REGISTRATION(CentaurMessageTestCase);

#define ZEROMQ_DATA_FACTOR		16

void CentaurMessageTestCase::testBSON()
{

	bson::bo objOrginal;
	{
		bson::bob objBuilder;
		objBuilder.append("name", "joe");
		objBuilder.append("age", 33.7);
		objOrginal = objBuilder.obj();
	}

	std::vector<char> compressedData;
	compressedData.resize(objOrginal.objsize() * 4);
	unsigned int compressedDataSize = compressedData.size();
	CPPUNIT_ASSERT_EQUAL(BZ_OK, BZ2_bzBuffToBuffCompress(compressedData.data(), &compressedDataSize, (char *)objOrginal.objdata(), objOrginal.objsize(), 9, 0, 30));
	compressedData.resize(compressedDataSize);

	std::vector<char> uncompressedData;
	uncompressedData.resize(compressedData.size() * 4);
	unsigned int uncompressedDataSize = uncompressedData.size();
	CPPUNIT_ASSERT_EQUAL(BZ_OK, BZ2_bzBuffToBuffDecompress(uncompressedData.data(), &uncompressedDataSize, (char *)compressedData.data(), compressedData.size(), 0, 0));
	uncompressedData.resize(uncompressedDataSize);

	CPPUNIT_ASSERT_EQUAL(uncompressedData.size(), (std::vector<char>::size_type)objOrginal.objsize());

	for (unsigned int i = 0; i < uncompressedData.size(); i++)
		CPPUNIT_ASSERT_EQUAL(uncompressedData.operator [](i), objOrginal.objdata()[i]);

	bson::bo objUnpacked(uncompressedData.data());

	CPPUNIT_ASSERT_EQUAL(std::string("joe"), objUnpacked.getField("name").String());
	CPPUNIT_ASSERT_EQUAL(33.7, objUnpacked.getField("age").Number());

}

void CentaurMessageTestCase::testZMQ_ReqRep_helper(std::vector<char> &dataOriginal, CentaurSocketRep &rep, CentaurSocketReq &req)
{
	std::vector<char> dataFromRequest;
	CPPUNIT_ASSERT_EQUAL(-1, rep.recv(dataFromRequest, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, rep.getError());

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), req.send(dataOriginal, true));

	for (int tries = 0; tries < 1000; tries++)
	{
		int rc = rep.recv(dataFromRequest, false);
		if (rc >= 0)
		{
			CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), rc);
			break;
		}
		CPPUNIT_ASSERT_EQUAL(EAGAIN, rep.getError());
		usleep(1);
	}
	CPPUNIT_ASSERT_EQUAL(dataOriginal.size(), dataFromRequest.size());
	CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.data(), dataFromRequest.data(), dataOriginal.size()));


	std::vector<char> dataFromResponse;
	CPPUNIT_ASSERT_EQUAL(-1, req.recv(dataFromResponse, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, req.getError());

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), rep.send(dataOriginal, true));

	for (int tries = 0; tries < 1000; tries++)
	{
		int rc = req.recv(dataFromResponse, false);
		if (rc >= 0)
		{
			CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), rc);
			break;
		}
		CPPUNIT_ASSERT_EQUAL(EAGAIN, req.getError());
		usleep(1);
	}
	CPPUNIT_ASSERT_EQUAL(dataOriginal.size(), dataFromResponse.size());
	CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.data(), dataFromResponse.data(), dataOriginal.size()));
}

void CentaurMessageTestCase::testZMQ_ReqRep()
{
	std::vector<char> dataOriginal;
	dataOriginal.resize(ZEROMQ_DATA_FACTOR * 256);

	for (int i = 0; i < ZEROMQ_DATA_FACTOR; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			int index = (i * 256) + j;
			CPPUNIT_ASSERT(index < (int)dataOriginal.size());
			dataOriginal.operator [](index) = j;
		}
	}

	CentaurSocketRep rep("tcp://127.0.0.1:5560");
	CPPUNIT_ASSERT(rep.open());

	CentaurSocketReq req("tcp://127.0.0.1:5560");
	CPPUNIT_ASSERT(req.open());

	testZMQ_ReqRep_helper(dataOriginal, rep, req);
}

void CentaurMessageTestCase::testZMQ_ReqRep_10000()
{

	struct timeval tv;
	gettimeofday(&tv, NULL);

	double t1 = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; // convert tv_sec & tv_usec to millisecond

	std::vector<char> dataOriginal;
	dataOriginal.resize(ZEROMQ_DATA_FACTOR * 256);

	for (int i = 0; i < ZEROMQ_DATA_FACTOR; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			int index = (i * 256) + j;
			CPPUNIT_ASSERT(index < (int)dataOriginal.size());
			dataOriginal.operator [](index) = j;
		}
	}

	CentaurSocketRep rep("tcp://127.0.0.1:5561");
	CPPUNIT_ASSERT(rep.open());

	CentaurSocketReq req("tcp://127.0.0.1:5561");
	CPPUNIT_ASSERT(req.open());

	for (int i = 0; i < 10000; i++)
	{
		if ((i % 1000) == 0 && i != 0)
		{
			struct timeval tv;
			gettimeofday(&tv, NULL);

			double diff = ((tv.tv_sec) * 1000 + (tv.tv_usec) / 1000) - t1; // convert tv_sec & tv_usec to millisecond

			std::cerr << i << " in " << diff << " milliseconds (" << diff / (double)i << " milliseconds per bounce) of " << dataOriginal.size() / 1024 << "KB" << "\n";
		}
		testZMQ_ReqRep_helper(dataOriginal, rep, req);
	}
}

void CentaurMessageTestCase::testZMQ_PubSub()
{
	std::vector<char> dataOriginal;
	dataOriginal.resize(ZEROMQ_DATA_FACTOR * 256);

	for (int i = 0; i < ZEROMQ_DATA_FACTOR; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			int index = (i * 256) + j;
			CPPUNIT_ASSERT(index < (int)dataOriginal.size());
			dataOriginal.operator [](index) = j;
		}
	}

	CentaurSocketPub pub("tcp://127.0.0.1:5562");
	CPPUNIT_ASSERT(pub.open());

	CentaurSocketSub sub("tcp://127.0.0.1:5562");
	CPPUNIT_ASSERT(sub.open(false));

	std::vector<char> dataFromSubscriber;
	CPPUNIT_ASSERT_EQUAL(-1, sub.recv(dataFromSubscriber, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, sub.getError());

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), pub.send(dataOriginal, true));

	CPPUNIT_ASSERT_EQUAL(-1, sub.recv(dataFromSubscriber, false));
	CPPUNIT_ASSERT_EQUAL(EAGAIN, sub.getError());

	const char * filter = "bob";
	CPPUNIT_ASSERT_EQUAL(0, sub.subscribe(filter, strlen(filter)));

	sleep(1); // give the socket the chance to apply the filter...

	memcpy(dataOriginal.data(), filter, strlen(filter));

	CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), pub.send(dataOriginal, true));

	for (int tries = 0; tries < 1000; tries++)
	{
		int rc = sub.recv(dataFromSubscriber, false);
		if (rc >= 0)
		{
			CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), rc);
			break;
		}
		CPPUNIT_ASSERT_EQUAL(EAGAIN, sub.getError());
		usleep(1);
	}
	CPPUNIT_ASSERT_EQUAL(dataOriginal.size(), dataFromSubscriber.size());
	CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.data(), dataFromSubscriber.data(), dataOriginal.size()));

}
