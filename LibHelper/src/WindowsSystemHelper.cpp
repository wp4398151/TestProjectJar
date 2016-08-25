#include "WindowsSystemHelper.h"
#include <Windows.h>
#include <string>
#include "Helper.h"

#include "gtest/gtest.h"

using namespace std;

TEST(TestGetFileSizeW, GetFileSizeW)
{
	wstring wstrFilePath;
	ASSERT_TRUE(GetAppPathW(wstrFilePath));
	wstrFilePath += L"\\test.mp4";
	DWORD dwFileSize = 0;
	ASSERT_TRUE(GetFileSizeW(wstrFilePath, dwFileSize));
	EXPECT_EQ(dwFileSize, 5036536);
}

/**
 * GetFileSize 
 */
bool GetFileSizeW(wstring& filePath, DWORD &fileSize)
{
	DWORD dwFileAccess = GENERIC_READ;
	DWORD dwShareAccess = FILE_SHARE_READ ;
	DWORD dwLastCode = 0;
	HANDLE hFile = CreateFileW(filePath.c_str(), dwFileAccess, dwShareAccess, NULL, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{ 
		if (dwLastCode = GetLastError())
		{
			DOLOGW(L"file "+ filePath.c_str() + L" not found!");
		}
		else
		{
			DOLOGW(L"file "+ filePath.c_str() + L" open error! last error: " + dwLastCode);
		}
		return false;
	}

	//UINT64 size = 0;
	DWORD dwLow = 0;
	//DWORD dwHigh = 0;
	//size |= ::GetFileSize(hFile, (DWORD*)(((BYTE*)&size) + 4));
	dwLow = ::GetFileSize(hFile, NULL);
	if (dwLow == INVALID_FILE_SIZE){
		DOLOGW(L"GetFile Size if failed!");
		CloseHandle(hFile);
		return false;
	}
	fileSize = dwLow;
	CloseHandle(hFile);
	return true;
}
