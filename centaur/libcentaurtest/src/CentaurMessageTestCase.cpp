#include "CentaurMessageTestCase.h"

#include <CentaurSockets.h>

#include <bson/bson.h>
#include <bzip2/bzlib.h>

#include <string.h>

CPPUNIT_TEST_SUITE_REGISTRATION( CentaurMessageTestCase );

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

void CentaurMessageTestCase::testZMQ()
{

	CentaurSocketRep rep("tcp://127.0.0.1:5560");
	CPPUNIT_ASSERT(rep.open());

	CentaurSocketReq req("tcp://127.0.0.1:5560");
	CPPUNIT_ASSERT(req.open());

	std::vector<char> dataOriginal;
	dataOriginal.resize(16 * 256);

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			int index = (i * 256) + j;
			CPPUNIT_ASSERT(index < (int)dataOriginal.size());
			dataOriginal.operator [](index) = j;
		}
	}

	{
		CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), req.send(dataOriginal, true));

		std::vector<char> dataFromRequest;
		CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), rep.recv(dataFromRequest, true));
		CPPUNIT_ASSERT_EQUAL(dataOriginal.size(), dataFromRequest.size());
		CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.data(), dataFromRequest.data(), dataOriginal.size()));

		CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), rep.send(dataOriginal, true));

		std::vector<char> dataFromResponse;
		CPPUNIT_ASSERT_EQUAL((int)dataOriginal.size(), req.recv(dataFromResponse, true));
		CPPUNIT_ASSERT_EQUAL(dataOriginal.size(), dataFromResponse.size());
		CPPUNIT_ASSERT_EQUAL(0, memcmp(dataOriginal.data(), dataFromResponse.data(), dataOriginal.size()));
	}

}
