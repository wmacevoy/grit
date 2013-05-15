#pragma once

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class CentaurMessageTestCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( CentaurMessageTestCase );
	CPPUNIT_TEST( testBSON );
	CPPUNIT_TEST_SUITE_END();

protected:

public:

protected:
	void testBSON();
};
