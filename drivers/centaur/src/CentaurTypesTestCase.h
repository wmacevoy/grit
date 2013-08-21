#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include <CentaurTypes.h>

class CentaurTypesTestCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(CentaurTypesTestCase);
	CPPUNIT_TEST(testCM_Array);
	CPPUNIT_TEST_SUITE_END();

protected:

public:

protected:

	void testCM_Array();
};
