#pragma once
#include "Helper.h"
#include <assert.h>

#include <vector>

////////////////////////////////////////////////////
// 简单的单元测试类 
// Wupeng 
// 实例:
//		UnitTest::Instance().testAll();
// deprecate!
////////////////////////////////////////////////////
#if 0
#define WPUNITTESTSTART(x, bActive) class Unit##x{		\
public:\
	Unit##x(){ if(bActive){ UnitTest::m_TestFuncList.push_back(UnitTest::test##x);} }\
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

WPUNITTESTSTART(GetAppPathA, false)
	string appPath;
	assert(GetAppPathA(appPath));
	OutputDebugStringA(appPath.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetAppPathW, false)
	wstring appPath;
	assert(GetAppPathW(appPath));
	OutputDebugStringW(appPath.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetAppNameA, false)
	string appNameA;
	assert(GetAppNameA(appNameA));
	OutputDebugStringA(appNameA.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetAppNameW, false)
	wstring appNameW;
	assert(GetAppNameW(appNameW));
	OutputDebugStringW(appNameW.c_str());
WPUNITTESTEND

WPUNITTESTSTART(GetCaptureScreenDCRGBbits, false)
	int width = 0;
	int height = 0;
	int platebitsCount = 0;
	char* lpRGBBits = GetCaptureScreenDCRGBbits(width, height, platebitsCount);
	ASSERT_NOTNULL(lpRGBBits);
	free(lpRGBBits);
WPUNITTESTEND

WPUNITTESTSTART(LoadTableFromFile, false)
	wstring appPath;
	wstring outputTableFile;
	assert(GetAppPathW(appPath));
	outputTableFile = (appPath + L"\\..\\MyTestGameClient\\inc\\GenerateTables\\UserTable.h");
	appPath += L"\\UserTable.txt";
	C2DimensionParser paser;
	paser.LoadTableFromFile(appPath, outputTableFile, false);
WPUNITTESTEND

WPUNITTESTSTART(LoadIndexTableFromFile, false)
	wstring appPath;
	wstring outputIndexTableFile;
	assert(GetAppPathW(appPath));
	outputIndexTableFile = (appPath + L"\\..\\MyTestGameClient\\inc\\GenerateTables\\UserIndexTable.h");

	appPath += L"\\UserIndexTable.txt";
	C2DimensionParser paser;
	paser.LoadTableFromFile(appPath, outputIndexTableFile, true);
WPUNITTESTEND

WPUNITTESTSTART(GetWindowsVersion, false)
	DWORD dwMajorVer = 0;
	DWORD dwMinorVer = 0;
	DWORD dwBuildNumber = 0;
	ASSERT_TRUE(GetNtVersionNumbers32(dwMajorVer, dwMinorVer, dwBuildNumber));
	DOLOG("windows version: ("+dwMajorVer + "." + dwMinorVer + "." + dwBuildNumber+")\r\n");
WPUNITTESTEND

WPUNITTESTSTART(GetElapse, false)

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

WPUNITTESTSTART(GetProcessModule, false)
	int processID = GetCurrentProcessId();
	list<string> moduleNames;
	ASSERT_TRUE(EnumSpecificProcessModule(processID, moduleNames));
WPUNITTESTEND

WPUNITTESTSTART(GetProcessModuleEx, false)
	int processID = GetCurrentProcessId();
	list<wstring> moduleNames;
	ASSERT_TRUE(EnumSpecificProcessModuleEx(processID, moduleNames));
WPUNITTESTEND

WPUNITTESTSTART(DisplayCallerAccessTokenInformation, false)
	ASSERT_TRUE(DisplayCallerAccessTokenInformation());
WPUNITTESTEND

WPUNITTESTSTART(GetAddressByDomain, false)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err = 0;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	ASSERT_EQU(err, 0);
	ULONG addr = getAddrByDomain("www.baidu.com");
	ASSERT_NOTZERO(addr);
	WSACleanup();
WPUNITTESTEND

WPUNITTESTSTART(EnumWindowsInSpecificProcess, false)
	Sleep(5000);
	DWORD procID = GetSpecificProcIDByNameEx("DMC-DevilMayCry.exe");
	EnumWindowsInSpecificProcess(procID);
WPUNITTESTEND

WPUNITTESTSTART(RegistryOp, true)
	RegistryOp myRegistry(HKEY_CURRENT_USER);
	myRegistry.Open(L"SOFTWARE\\Lenovo\\NerveCenter\\Setting");
	int yourage = false;

	ASSERT_TRUE(myRegistry.Write(L"yourage", 12));

	ASSERT_TRUE(myRegistry.Read(L"yourage", yourage));
	DOLOG("SOFTWARE\\Lenovo\\NerveCenter\\Setting\\yourage : " + yourage);
	//DOLOG("HighQualityResolutionTimeLite Lapse: " + timelapse + "micro sec\r\n");
	wstring name;
	ASSERT_TRUE(myRegistry.Read(L"myname", name));
	DOLOGW(L"SOFTWARE\\Lenovo\\NerveCenter\\Setting\\myname : " + name);

	ASSERT_TRUE(myRegistry.Write(L"yourname", name.c_str()));

	ASSERT_TRUE(myRegistry.DeleteValue(L"yourage"));

	ASSERT_TRUE(myRegistry.CreateKey(L"SubSetting"));

WPUNITTESTEND

};
#endif

/**
 * WPTestCaseBase
 * all test case will derive it.
 */
class TestCaseBase
{
public:
	TestCaseBase(LPCSTR lpCaseName) : m_CaseName(lpCaseName), m_bResult(true){}

	LPCSTR m_CaseName;
	virtual void Run() = 0;
	BOOL m_bResult;
};

/**
 * WPUnitTestClass  
 * all test case must regist in the class.
 */
class UnitTest : public CLiteSingleton<UnitTest>
{
public:
	friend class CLiteSingleton<UnitTest>;
	TestCaseBase* RegisterTestCase(TestCaseBase* testcase);
	int Run();

	TestCaseBase* m_pCurrentTestCase;
	BOOL m_bTestResult;
	int m_nPassedCount;
	int m_nFailedCount;

protected:
	std::vector<TestCaseBase*> m_TestCases; // 案例集合
};

#define TESTCASE_NAME(nameTest) \
	nameTest##_TEST

#define WPUNITTESTSTART(nameTest) \
class TESTCASE_NAME(nameTest) : public TestCaseBase \
{ \
public: \
	TESTCASE_NAME(nameTest)(const char* pCaseName) : TestCaseBase(pCaseName){ DOLOG("!!!!New "+ pCaseName)}; \
	virtual void Run(); \
private: \
	static TestCaseBase* const s_pTestcase; \
}; \
	\
	TestCaseBase* const TESTCASE_NAME(nameTest) \
	::s_pTestcase = UnitTest::Instance().RegisterTestCase(\
	new TESTCASE_NAME(nameTest)(#nameTest)); \
	void TESTCASE_NAME(nameTest)::Run()

#define WPEXPECT_EQ(m, n) \
if (m != n) \
{ \
	UnitTest::Instance().m_pCurrentTestCase->m_bResult = false; \
	DOLOG("Failed!"); \
	DOLOG("Expect:" + m); \
	DOLOG("Actual:" + n); \
}

#define WPEXPECT_TRUE(m) \
if (m != TRUE) \
{ \
	UnitTest::Instance().m_pCurrentTestCase->m_bResult = false; \
	DOLOG("Failed!"); \
	DOLOG("Expect:true" + m); \
	DOLOG("Actual:false"); \
}
