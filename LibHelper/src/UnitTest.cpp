////////////////////////////////////////////////////
// 用来进简单测试的类
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

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

// TODO: add libevent
bool UnitTest::StartSimpleLibeventServer()
{
	return true;
}

bool UnitTest::StartSimpleLibeventClient()
{
	return true;
}

// TODO: add plain socket class
bool UnitTest::StartPlainSocketServer()
{
	return true;
}

bool UnitTest::StartPlainSocketClient()
{
	return true;
}

// TODO: add plain socket select class
bool UnitTest::StartSelectSocketServer()
{
	return true;
}

bool UnitTest::StartSelectSocketClient()
{
	return true;
}