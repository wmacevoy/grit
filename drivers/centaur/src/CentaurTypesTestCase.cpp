#include "CentaurTypesTestCase.h"

#include <string.h>

CPPUNIT_TEST_SUITE_REGISTRATION(CentaurTypesTestCase);

void CentaurTypesTestCase::testCM_Array()
{
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
		CM_Array<char, 2048> cmArray;

		for (int i = 0; i < (int)dataOriginal.size(); i++)
		{
			int count = i + 1;

			cmArray.setCount(count);
			CPPUNIT_ASSERT_EQUAL(count, cmArray.getCount());
			CPPUNIT_ASSERT_EQUAL(sizeof(char) * count, cmArray.getSize());

			cmArray[i] = dataOriginal[i];
			CPPUNIT_ASSERT_EQUAL(0, memcmp(cmArray.getData(), dataOriginal.data(), count));
		}
	}

	{
		CM_Array<char, 2048> cmArray;

		for (int i = (int)dataOriginal.size(); i-->0; )
		{
			int count = i + 1;

			cmArray.setCount(count);
			CPPUNIT_ASSERT_EQUAL(count, cmArray.getCount());
			CPPUNIT_ASSERT_EQUAL(sizeof(char) * count, cmArray.getSize());

			if (i == (int)dataOriginal.size() - 1)
				memcpy(cmArray.getData(), dataOriginal.data(), count);

			CPPUNIT_ASSERT_EQUAL(0, memcmp(cmArray.getData(), dataOriginal.data(), count));
		}
	}

}
