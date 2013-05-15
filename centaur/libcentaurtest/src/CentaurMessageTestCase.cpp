#include "CentaurMessageTestCase.h"

#include <bson/bson.h>

CPPUNIT_TEST_SUITE_REGISTRATION( CentaurMessageTestCase );

void CentaurMessageTestCase::testBSON()
{

	bson::bo empty;
	cout << "empty: " << empty.hexDump() << endl;

	/* make a simple { name : 'joe', age : 33.7 } object */
	{
		bson::bob b;
		b.append("name", "joe");
		b.append("age", 33.7);
		cout << "b: " << b.obj().hexDump() << endl;
	}

}
