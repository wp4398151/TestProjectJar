#include "MiniDump.h"
#include <Windows.h>
#include <Dbghelp.h>
#include <string>
#include <time.h>
using namespace std;

#pragma comment(lib, "Dbghelp.lib")

inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
{
	if (pModuleName == 0)
	{
		return FALSE;
	}

	WCHAR szFileName[_MAX_FNAME] = L"";
	_wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);

	if (_wcsicmp(szFileName, L"ntdll") == 0)
		return TRUE;

	return FALSE;
}

inline BOOL CALLBACK MiniDumpCallback(PVOID                            pParam,
	const PMINIDUMP_CALLBACK_INPUT   pInput,
	PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
	if (pInput == 0 || pOutput == 0)
		return FALSE;

	switch (pInput->CallbackType)
	{
	case ModuleCallback:
		if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
			if (!IsDataSectionNeeded(pInput->Module.FullPath))
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		return TRUE;
	default:;
	}

	return FALSE;
}

inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
{
	HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = NULL;

		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		mci.CallbackParam = 0;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pep != 0) ? &mdei : 0, NULL, &mci);

		CloseHandle(hFile);
	}
}

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	time_t curTime;
	time(&curTime);
	tm timeStruct;
	localtime_s(&timeStruct, &curTime);
	timeStruct.tm_hour = 0;
	timeStruct.tm_min = 0;
	timeStruct.tm_sec = 0;
	timeStruct.tm_mday += 1;

	wstring dumpFileName = L"coredump-";
	dumpFileName += to_wstring(timeStruct.tm_year);
	dumpFileName += L"-";
	dumpFileName += to_wstring(timeStruct.tm_mon);
	dumpFileName += L"-";
	dumpFileName += to_wstring(timeStruct.tm_mday);
	dumpFileName += L"-";
	dumpFileName += to_wstring(timeStruct.tm_hour);
	dumpFileName += L"-";
	dumpFileName += to_wstring(timeStruct.tm_min);
	dumpFileName += L"-";
	dumpFileName += to_wstring(timeStruct.tm_sec);
	dumpFileName += L".dmp";

	CreateMiniDump(pExceptionInfo, dumpFileName.c_str());

	return EXCEPTION_EXECUTE_HANDLER;
}

/**
 * 为了防止SetUnhandledExceptionFilter被编译器重新调用，因此修改该入口,使得只要初始化之后，自定义的异常处理毁掉就会被执行。.
 */
void DisableSetUnhandledExceptionFilter()
{
	void* addr = (void*)GetProcAddress(LoadLibrary(L"kernel32.dll"),
		"SetUnhandledExceptionFilter");

	if (addr)
	{
		unsigned char code[16];
		int size = 0;

		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

void InitMinDump()
{
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	DisableSetUnhandledExceptionFilter();
}