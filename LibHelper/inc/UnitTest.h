#pragma once
#include "Helper.h"
#include <list>
#include <assert.h>
#include <Winsock2.h>

#include "C2DimensionParser.h"
#include "SecurityHelper.h"

////////////////////////////////////////////////////
// �򵥵ĵ�Ԫ������ 
// Wupeng 
// ʵ��:
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

	static bool StartSimpleLibeventServer();
	static bool StartSimpleLibeventClient();
	static bool StartPlainSocketServer();
	static bool StartPlainSocketClient();
	static bool StartSelectSocketServer();
	static bool StartSelectSocketClient();

	static bool ShowATopMostInfoWindow(wstring &showText);

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
	wstring outputTableFile;
	assert(GetAppPathW(appPath));
	outputTableFile = (appPath + L"\\..\\MyTestGameClient\\inc\\GenerateTables\\UserTable.h");
	appPath += L"\\UserTable.txt";
	C2DimensionParser paser;
	paser.LoadTableFromFile(appPath, outputTableFile, false);
WPUNITTESTEND

WPUNITTESTSTART(LoadIndexTableFromFile)
	wstring appPath;
	wstring outputIndexTableFile;
	assert(GetAppPathW(appPath));
	outputIndexTableFile = (appPath + L"\\..\\MyTestGameClient\\inc\\GenerateTables\\UserIndexTable.h");

	appPath += L"\\UserIndexTable.txt";
	C2DimensionParser paser;
	paser.LoadTableFromFile(appPath, outputIndexTableFile, true);
WPUNITTESTEND

WPUNITTESTSTART(GetWindowsVersion)
	DWORD dwMajorVer = 0;
	DWORD dwMinorVer = 0;
	DWORD dwBuildNumber = 0;
	ASSERT_TRUE(GetNtVersionNumbers32(dwMajorVer, dwMinorVer, dwBuildNumber));
	DOLOG("windows version: ("+dwMajorVer + "." + dwMinorVer + "." + dwBuildNumber+")\r\n");
WPUNITTESTEND

WPUNITTESTSTART(GetElapse)

	INT startCount = GetTickCount();
	int a = 0;
	for (LONGLONG i = 0; i < 1000; ++i)
	{
		a += i;
	}
	startCount = GetTickCount() - startCount;

	DOLOG("GetTickCount Lapse: " + startCount + "mili sec \r\n");

	HighQualityResolutionTimeLite timeLite;
	timeLite.Reset();
	for (LONGLONG i = 0; i < 1000; ++i)
	{
		a += i;
	}
	int b = a;
	LONGLONG timelapse = timeLite.GetTimelapse();
	DOLOG("HighQualityResolutionTimeLite Lapse: "+ timelapse + "micro sec\r\n");
WPUNITTESTEND

WPUNITTESTSTART(GetProcessModule)
	int processID = GetCurrentProcessId();
	list<string> moduleNames;
	ASSERT_TRUE(EnumSpecificProcessModule(processID, moduleNames));
WPUNITTESTEND

WPUNITTESTSTART(GetProcessModuleEx)
	int processID = GetCurrentProcessId();
	list<wstring> moduleNames;
	ASSERT_TRUE(EnumSpecificProcessModuleEx(processID, moduleNames))
WPUNITTESTEND

WPUNITTESTSTART(DisplayCallerAccessTokenInformation)
	ASSERT_TRUE(DisplayCallerAccessTokenInformation());
WPUNITTESTEND

WPUNITTESTSTART(GetAddressByDomain)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err = 0;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	ASSERT_EQU(err, 0);
	ULONG addr = getAddrByDomain("www.baidu.com");
	ASSERT_NOTZERO(addr);
	WSACleanup();
WPUNITTESTEND};
