#include "UnitTest.h"

std::list<UNITTESTPROC> UnitTest::m_TestFuncList;

UnitTest::UnitTest()
{
}

UnitTest::~UnitTest()
{

}

void UnitTest::testAll()
{
	for (std::list<UNITTESTPROC>::iterator itor = m_TestFuncList.begin();
		itor != m_TestFuncList.end(); ++itor)
	{
		(*itor)();
	}
}

