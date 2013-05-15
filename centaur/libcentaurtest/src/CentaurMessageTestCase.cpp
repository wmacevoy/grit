#include "CentaurMessageTestCase.h"

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
