#pragma once
#include "Helper.h"
#include <list>
#include <assert.h>
#include "C2DimensionParser.h"

////////////////////////////////////////////////////
// 简单的单元测试类 
// Wupeng 
// 实例:
//		UnitTest::Instance().testAll();
////////////////////////////////////////////////////

#define WPUNITTESTSTART(x) class Unit##x{		\
public:\
	Unit##x(){ UnitTest::m_TestFuncList.push_back(UnitTest::test##x); }\
} m_Unit##x; \
	static void test##x(){	\
		OutputDebugStringA("-----------("###x##" Test Start)---------\r\n");
#define WPUNITTESTEND OutputDebugStringW(L"\r\n------------END---------\r\n"); }

typedef void(*UNITTESTPROC)();

class UnitTest : public CLiteSingleton<UnitTest>
{
	friend class CLiteSingleton<UnitTest>;
private:
	UnitTest();
	~UnitTest();
public:
	void testAll();
public:
	static std::list<UNITTESTPROC> m_TestFuncList;

WPUNITTESTSTART(GetAppPathA)
		string appPath;
		assert(GetAppPathA(appPath));
		OutputDebugStringA(appPath.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetAppPathW)
			wstring appPath;
			assert(GetAppPathW(appPath));
			OutputDebugStringW(appPath.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetAppNameA)
			string appNameA;
			assert(GetAppNameA(appNameA));
			OutputDebugStringA(appNameA.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetAppNameW)
			wstring appNameW;
			assert(GetAppNameW(appNameW));
			OutputDebugStringW(appNameW.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetCaptureScreenDCRGBbits)
	int width = 0;
	int height = 0;
	int platebitsCount = 0;
	char* lpRGBBits = GetCaptureScreenDCRGBbits(width, height, platebitsCount);
	ASSERT_NOTNULL(lpRGBBits);
	free(lpRGBBits);
WPUNITTESTEND


WPUNITTESTSTART(LoadTableFromFile)

	wstring appPath;
	assert(GetAppPathW(appPath));
	appPath += L"\\UserTable.txt";
	C2DimensionParser paser;
	paser.LoadTableFromFile(appPath);
WPUNITTESTEND
};
