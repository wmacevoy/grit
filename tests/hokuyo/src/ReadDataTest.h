#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ReadDataTestCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(ReadDataTestCase);
	CPPUNIT_TEST(testReadingData);
	CPPUNIT_TEST_SUITE_END();

protected:

public:

protected:

	void testReadingData();
};
