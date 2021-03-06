#include "Helper.h"
#include <Windows.h>
#include <iostream>

using namespace std;

// 判断大小端绪
// 简单判定 小端是低低高高，数的低位放在低内存，高位放在高内存
bool IsByteLittleEndian(bool &bIsLittleEndian)
{
	union
	{
		short value;
		char union_bytes[sizeof(short)];
	} test;
	test.value = 0x0102;
	if ((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2))
	{
		bIsLittleEndian = false;
		return true;
	}
	else if ((test.union_bytes[0] == 2) && (test.union_bytes[1] == 1))
	{
		bIsLittleEndian = true;
		return true;
	}
	else
	{
		return false;
	}
}

bool GetAppNameW(wstring& strPath)
{
	TCHAR modulePath[MAX_PATH];
	if (0 == GetModuleFileNameW(NULL, modulePath, MAX_PATH))
	{
		DOLOG("Get Current Module Name Failed!");
		return false;
	}
	strPath = modulePath;
	int pos = strPath.find_last_of(L'\\');
	if (wstring::npos == pos)
	{
		return false;
	}
	int count = strPath.size() - pos;
	strPath = strPath.substr(strPath.find_last_of(L'\\')+1, count);
	return true;
}

// 得到当前程序的EXE名字
bool GetAppNameA(string& strPath)
{
	char modulePath[MAX_PATH];
	if (0 == GetModuleFileNameA(NULL, modulePath, MAX_PATH))
	{
		DOLOG("Get Current Module Name Failed!");
		return false;
	}
	strPath = modulePath;
	int pos = strPath.find_last_of('\\');
	if (string::npos == pos)
	{
		return false;
	}
	int count = strPath.size() - pos;
	strPath = strPath.substr(strPath.find_last_of('\\')+1, count);
	return true;
}

// 得到当前程序的路径
bool GetAppPathW(wstring& strPath)
{
	TCHAR modulePath[MAX_PATH];
	if (0 == GetModuleFileNameW(NULL, modulePath, MAX_PATH))
	{
		DOLOG("Get Current Module Name Failed!");
		return false;
	}
	strPath = modulePath;
	strPath  = strPath.substr(0, strPath.find_last_of(L'\\'));
	return true;
}

// 得到当前程序的路径
bool GetAppPathA(string& strPath)
{
	char modulePath[MAX_PATH];
	if (0 == GetModuleFileNameA(NULL, modulePath, MAX_PATH))
	{
		DOLOG("Get Current Module Name Failed!");
		return false;
	}
	strPath = modulePath;
	strPath  = strPath.substr(0, strPath.find_last_of('\\'));
	return true;
}

/**
 * 去掉文件全名称中的路径，仅留下文件名称
 */
bool TrimFilePathA(string& strPath)
{
	int pos = strPath.find_last_of('\\');
	if (string::npos == pos)
	{
		return false;
	}
	int count = strPath.size() - pos;
	strPath = strPath.substr(strPath.find_last_of('\\') + 1, count);
	return true;
}

bool TrimFilePathW(wstring& strPath)
{
	int pos = strPath.find_last_of(L'\\');
	if (wstring::npos == pos)
	{
		return false;
	}
	int count = strPath.size() - pos;
	strPath = strPath.substr(strPath.find_last_of(L'\\') + 1, count);
	return true;
}

string &ReplaceAll(string& rSrcString, string& rSearchString, string& rReplaceString)
{
	string::size_type pos = 0;
	while ((pos = rSrcString.find(rSearchString, pos)) != rSrcString.npos) {
		rSrcString.replace(pos, rSearchString.size(), rReplaceString);
		pos += rReplaceString.length();
	}
	return rSrcString;
}

////////////////////////////////////////////////////
// widechar Unicode 转换成UTF8, 目前只支持
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertWidechar2UTF8(const wstring &rWStr, string &rStr)
{
	int iReceiveSize = 0;
	iReceiveSize = WideCharToMultiByte(CP_UTF8,		// 多字节的代码页
		0,				// 控制转换细节，0不控制
		rWStr.c_str(),	// 需要转换的款字节字符串
		-1,				// 宽字节字符串的长度
		NULL,			// 获得转换之后的字符串的缓存
		0,				// 缓存的大小
		NULL,			// 不能转换的默认字符
		NULL);			// 是否完全转换成功
	if (0 == iReceiveSize)
	{
		int icode = GetLastError();
		DOLOG("计算字符串转换字节数失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	char bufChar[kConvertCodepageCacheLen] = { 0 };

	if (0 == WideCharToMultiByte(
		CP_UTF8,	// 多字节的代码页
		0,			// 控制转换细节，0不控制
		rWStr.c_str(),			// 需要转换的款字节字符串
		-1,// 宽字节字符串的长度
		bufChar,// 获得转换之后的字符串的缓存
		iReceiveSize,// 缓存的大小
		NULL,// 不能转换的默认字符
		NULL// 是否完全转换成功
		))
	{
		DOLOG("宽字符转换UTF8错误");
		return FALSE;
	}
	rStr = bufChar;
	return TRUE;
}

BOOL ConvertWidechar2ANSIMalloc(const wchar_t* pwStr, char** pStr)
{
	ASSERT_NOTNULLRET(pwStr, FALSE);
	ASSERT_NOTNULLRET(pStr, FALSE);

	int iReceiveSize = 0;
	iReceiveSize = WideCharToMultiByte(CP_ACP,		// 多字节的代码页
							0,				// 控制转换细节，0不控制
							pwStr,	// 需要转换的款字节字符串
							-1,				// 宽字节字符串的长度
							NULL,			// 获得转换之后的字符串的缓存
							0,				// 缓存的大小
							NULL,			// 不能转换的默认字符
							NULL);			// 是否完全转换成功

	if (0 == iReceiveSize)
	{
		DOLOG("计算字符串转换字节数失败");
		return FALSE;
	}
	char* pTempAnsiStr = (char*)malloc(iReceiveSize);
	ZeroMemory(pTempAnsiStr, iReceiveSize);
	if (0 == WideCharToMultiByte(
		CP_ACP,					// 多字节的代码页
		0,						// 控制转换细节，0不控制
		pwStr,					// 需要转换的宽字节字符串
		-1,						// 宽字节字符串的长度
		pTempAnsiStr,			// 获得转换之后的字符串的缓存
		iReceiveSize,			// 缓存的大小
		NULL,					// 不能转换的默认字符
		NULL					// 是否完全转换成功
		))
	{
		DOLOG("宽字符转换ANSI错误");
		return FALSE;
	}
	*pStr = pTempAnsiStr;
	return TRUE;
}

////////////////////////////////////////////////////
// widechar Unicode 转换成ANSI
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertWidechar2ANSI(const wstring &rWStr, string &rStr)
{
	int iReceiveSize = 0;
	iReceiveSize = WideCharToMultiByte(CP_ACP,		// 多字节的代码页
							0,				// 控制转换细节，0不控制
							rWStr.c_str(),	// 需要转换的款字节字符串
							-1,				// 宽字节字符串的长度
							NULL,			// 获得转换之后的字符串的缓存
							0,				// 缓存的大小
							NULL,			// 不能转换的默认字符
							NULL);			// 是否完全转换成功
	if (0 == iReceiveSize)
	{
		DOLOG("计算字符串转换字节数失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	CHAR bufChar[kConvertCodepageCacheLen] = { 0 };

	if (0 == WideCharToMultiByte(
		CP_ACP,					// 多字节的代码页
		0,						// 控制转换细节，0不控制
		rWStr.c_str(),			// 需要转换的款字节字符串
		-1,						// 宽字节字符串的长度
		bufChar,				// 获得转换之后的字符串的缓存
		iReceiveSize,			// 缓存的大小
		NULL,					// 不能转换的默认字符
		NULL					// 是否完全转换成功
		))
	{
		DOLOG("宽字符转换ANSI错误");
		return FALSE;
	}
	rStr = bufChar;
	return TRUE;
}

////////////////////////////////////////////////////
// UTF8 转换成WideChar Unicode
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertUTF82Widechar(const string& rStr, wstring& rWStr)
{
	int iReceiveSize = 0;
	iReceiveSize = MultiByteToWideChar(CP_UTF8,		// 多字节的代码页
										0,				// 控制转换细节，0不控制
										rStr.c_str(),	// 需要转换的款字节字符串
										-1,				// 宽字节字符串的长度
										NULL,			// 获得转换之后的字符串的缓存
										0);				// 缓存的大小
	if (0 == iReceiveSize)
	{
		DOLOG("计算字符串转换字节数失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	WCHAR bufChar[kConvertCodepageCacheLen*sizeof(WCHAR)] = { 0 };

	if (0 == MultiByteToWideChar(
		CP_UTF8,	// 多字节的代码页
		0,			// 控制转换细节，0不控制
		rStr.c_str(),			// 需要转换的款字节字符串
		-1,// 宽字节字符串的长度
		bufChar,// 获得转换之后的字符串的缓存
		kConvertCodepageCacheLen*sizeof(WCHAR)))
	{
		DOLOG("ANSI转换宽字符错误");
		return FALSE;
	}
	rWStr = bufChar;

	return TRUE;
}

////////////////////////////////////////////////////
// ANSI转换成WideChar Unicode
// Wupeng 
////////////////////////////////////////////////////
BOOL ConvertANSI2Widechar(const string& rStr, wstring& rWStr)
{
	int iReceiveSize = 0;
	iReceiveSize = MultiByteToWideChar(CP_ACP,		// 多字节的代码页
		0,				// 控制转换细节，0不控制
		rStr.c_str(),	// 需要转换的款字节字符串
		-1,				// 宽字节字符串的长度
		NULL,			// 获得转换之后的字符串的缓存
		0);				// 缓存的大小
	if (0 == iReceiveSize)
	{
		DOLOG("计算字符串转换字节数失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	WCHAR bufChar[kConvertCodepageCacheLen*sizeof(WCHAR)] = { 0 };

	if (0 == MultiByteToWideChar(
		CP_UTF8,	// 多字节的代码页
		0,			// 控制转换细节，0不控制
		rStr.c_str(),			// 需要转换的款字节字符串
		-1,// 宽字节字符串的长度
		bufChar,// 获得转换之后的字符串的缓存
		kConvertCodepageCacheLen*sizeof(WCHAR)))
	{
		DOLOG("ANSI转换宽字符错误");
		return FALSE;
	}
	rWStr = bufChar;

	return TRUE;
}

////////////////////////////////////////////////////
// ANSI转换成WideChar Unicode,使用CRT,未实现
// Wupeng 
////////////////////////////////////////////////////
BOOL CRTConvertANSI2Widechar(const string& rStr, wstring& rWStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.ACP");
	char *szLocale = setlocale(LC_ALL, ".ACP");

	if ( NULL == szLocale )
	{
		DOLOG("请求转换之后的字符串长度失败 " + szLocale);
		return FALSE;
	}

	size_t iReceiveSize = mbstowcs(NULL, rStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOLOG("请求转换之后的字符串长度失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	wchar_t bufChar[kConvertCodepageCacheLen+1] = { 0 };
		
	mbstowcs(bufChar, rStr.c_str(), iReceiveSize + 1);
	rWStr = bufChar;
	return TRUE;
}

BOOL CRTConvertUTF82Widechar(const string& rStr, wstring& rWStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.UTF-8");
	char *szLocale = setlocale(LC_ALL, ".UTF-8");
	
	if ( NULL == szLocale )
	{
		DOLOG("请求转换之后的字符串长度失败 " + szLocale);
		return FALSE;
	}

	size_t iReceiveSize = mbstowcs(NULL, rStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOLOG("请求转换之后的字符串长度失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	wchar_t bufChar[kConvertCodepageCacheLen+1] = { 0 };
		
	mbstowcs(bufChar, rStr.c_str(), iReceiveSize + 1);
	rWStr = bufChar;
	return TRUE;
}

BOOL CRTConvertWidechar2UTF8(const wstring& rWStr, string& rStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.UTF-8");
	char *szLocale = setlocale(LC_ALL, ".UTF-8");
	if ( NULL == szLocale )
	{
		DOLOG("请求转换之后的字符串长度失败 " + szLocale);
		return FALSE;
	}

	size_t iReceiveSize = wcstombs(NULL, rWStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOLOG("请求转换之后的字符串长度失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	char bufChar[kConvertCodepageCacheLen+1] = { 0 };
	wcstombs(bufChar, rWStr.c_str(), iReceiveSize + 1);
	rStr = bufChar;
	return TRUE;
}

BOOL CopyStringToWString(wstring &targetWStr, string &srcStr)
{
	WCHAR* wStrBuffer = (WCHAR*)malloc(srcStr.size() + 2);
	ASSERT_NOTNULLRET(wStrBuffer, FALSE);
	ZeroMemory(wStrBuffer, srcStr.size() + 2);
	memcpy((char*)wStrBuffer, srcStr.c_str(), srcStr.size());
	targetWStr = wStrBuffer;
	free(wStrBuffer);
	return TRUE;
}

BOOL CRTConvertWidechar2ANSI(const wstring& rWStr, string& rStr)
{
	//char *psz = setlocale(LC_ALL,"Chinese_People's Republic of China.936")
	//char *szLocale = setlocale(LC_ALL, "zh_CN.ACP");
	char *szLocale = setlocale(LC_ALL, ".ACP");
	
	if ( NULL == szLocale )
	{
		DOLOG("请求转换之后的字符串长度失败 " + szLocale);
		return FALSE;
	}

	size_t iReceiveSize = wcstombs(NULL, rWStr.c_str(), 0);
	
	if (iReceiveSize <= 0)
	{
		DOLOG("请求转换之后的字符串长度失败");
		return FALSE;
	}

	if (kConvertCodepageCacheLen < iReceiveSize)
	{
		DOLOG("字符串的长度超出了缓存的大小");
		return FALSE;
	}
	char bufChar[kConvertCodepageCacheLen+1] = { 0 };
	wcstombs(bufChar, rWStr.c_str(), iReceiveSize + 1);
	rStr = bufChar;
	return TRUE;
}

/*
测试代码
//	wstring wstr = L"12345";
//	string str = "12345";
//	string strTar;
//	wstring wstrTar;
//	ConvertWidechar2UTF8(wstr, strTar);
//	ConvertWidechar2ANSI(wstr, strTar);
//	ConvertUTF82Widechar(str, wstrTar);
//	ConvertANSI2Widechar(str, wstrTar);

//	CRTConvertWidechar2UTF8(wstr, strTar);
//	CRTConvertWidechar2ANSI(wstr, strTar);
//	CRTConvertANSI2Widechar(str, wstrTar);
//	CRTConvertANSI2Widechar(str, wstrTar);
//	CRTConvertUTF82Widechar(str, wstrTar);
*/

void TimeManager::CurrentDateTimeString(string& strTime)
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80] = { 0 };
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d, %X: ", &tstruct);
	strTime = buf;
}

void TimeManager::CurrentTimeString(string& strTime)
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80] = { 0 };
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%X: ", &tstruct);
	strTime = buf;
}

void TimeManager::PrintTime(time_t atime)
{
	tm timeStruct;
	char buff[32];
	localtime_s(&timeStruct, &atime);
	asctime_s(buff, &timeStruct);
	cout << buff << endl;
}

void TimeManager::GetCurTimeStr(string& strTime, time_t atime)
{
	tm timeStruct;
	char buff[32];
	localtime_s(&timeStruct, &atime);
	asctime_s(buff, &timeStruct);
	strTime = buff;
}

time_t TimeManager::GetOverDayTime()
{
	time_t curTime;
	time(&curTime);
	tm timeStruct;
	localtime_s(&timeStruct, &curTime);
	timeStruct.tm_hour = 0;
	timeStruct.tm_min = 0;
	timeStruct.tm_sec = 0;
	timeStruct.tm_mday += 1;
	return mktime(&timeStruct);
}

time_t TimeManager::GetOverDayTime(time_t curTime)
{
	tm timeStruct;
	localtime_s(&timeStruct, &curTime);
	timeStruct.tm_hour = 0;
	timeStruct.tm_min = 0;
	timeStruct.tm_sec = 0;
	timeStruct.tm_mday += 1;
	return mktime(&timeStruct);
}