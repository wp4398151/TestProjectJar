#include "Helper.h"
#include <windows.h>
#include <Psapi.h>

#undef UNICODE
#include <Tlhelp32.h>
#undef UNICODE
#include <tchar.h>

// [2015/12/17 wupeng] 
// test
//char tempbuf[max_path] = { 0 };
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", 2);
//dolog(tempbuf);
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", 50);
//dolog(tempbuf);
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", strlen("my.dll"));
//dolog(tempbuf);
//insertstrinstrspecifyposa(tempbuf, max_path, "my.dll", "64", 0);
//dolog(tempbuf);
BOOL InsertStrInStrSpecifyPosA(LPSTR lpTarBuf, UINT cBufLen, LPSTR lpSrcStr, LPSTR lpInsertStr, UINT iPos)
{
	if (ISNULL(lpTarBuf)){
		DOLOG("lpTarBuf NULL !");
		return FALSE;
	}

	if (ISNULL(lpSrcStr)){
		DOLOG("lpSrcStr NULL !");
		return FALSE;
	}

	if (ISNULL(lpInsertStr)){
		DOLOG("lpInsertStr NULL !");
		return FALSE;
	}
	
	UINT cInsertStrLen = (UINT)strlen(lpInsertStr);
	UINT cSrcStrLen = (UINT)strlen(lpSrcStr);

	if (iPos > cSrcStrLen)
	{
		DOLOG("iPos stack overflow !");
		return FALSE;
	}
	ZeroMemory(lpTarBuf, cBufLen);
	UINT cTotalNeed = cSrcStrLen + cInsertStrLen  + 1;

	if (cTotalNeed > cBufLen)
	{
		DOLOG("not enough for fill the result string!");
		return FALSE;
	}
	
	UINT uCurPos = 0;
	memcpy(lpTarBuf, lpSrcStr, iPos);
	uCurPos += iPos;
	memcpy(lpTarBuf + uCurPos, lpInsertStr, cInsertStrLen);
	uCurPos += cInsertStrLen;
	memcpy(lpTarBuf + uCurPos, lpSrcStr + iPos, cSrcStrLen - iPos);
		
	return TRUE;
}

// [2015/12/15 wupeng] 
// if failed return 0, success return the Process ID
DWORD GetSpecificProcIDByName(LPSTR lpName)
{
	DWORD resProcessID = 0;
	if (ISNULL(lpName))
	{ 
		DOLOG("Process Name can't be null");
		return 0;
	}

	if (ISFALSE(EnableDebugPrivilege(TRUE)))
	{
		DOLOG("EnableDebugPrivilege Failed !");
		return 0;
	}

	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32, sizeof(pe32));

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		DOLOG(" CreateToolhelp32Snapshot Failed ！ \n" + GetLastError());
		return 0;
	}

	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		DOLOG(" >>" + pe32.szExeFile + " : " + pe32.th32ProcessID);
		if (strcmp(pe32.szExeFile, lpName) == 0)
		{
			resProcessID = pe32.th32ProcessID;
		}
		bMore = Process32Next(hProcessSnap, &pe32);
	}

	// 不要忘记清除掉snapshot对象  
	CloseHandle(hProcessSnap);
	return resProcessID;
}

// [2015/12/15 wupeng] 
// if failed return 0, success return the Process ID
DWORD GetSpecificProcIDByNameEx(LPSTR lpName)
{
	if (ISNULL(lpName))
	{ 
		DOLOG("Process Name can't be null");
		return 0;
	}
	
	if (ISFALSE(EnableDebugPrivilege(TRUE)))
	{
		DOLOG("EnableDebugPrivilege Failed !");
		return 0;
	}

	DWORD allProcessIDs[2048] = { 0 };
	DWORD cbNeed = 0;

	if (ISZERO(EnumProcesses(allProcessIDs, sizeof(allProcessIDs), &cbNeed)))
	{
		DOLOG("EnumProcess Failed ! " + GetLastError());
		return 0;
	}

	HANDLE hProcess = NULL;
	CHAR szProcessName[MAX_PATH] = { 0 };
	for (DWORD i = 0; i < cbNeed; i++)
	{ 
		if (ISZERO(allProcessIDs[i]))
		{
			continue;
		}

		ZeroMemory(szProcessName, sizeof(szProcessName));
		hProcess = OpenProcess(
			PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE,
				allProcessIDs[i]);
		if (ISNOTNULL(hProcess))
		{
			if (ISNOZERO(GetModuleBaseNameA(hProcess, NULL, szProcessName,
				sizeof(szProcessName))))
			{
				DOLOG(" >>" + szProcessName + " : " + allProcessIDs[i]);
				if (ISZERO(strcmp(szProcessName, lpName)))
				{
					DOLOG("Found the Process " + lpName);
					CloseHandle(hProcess);
					return allProcessIDs[i];
				}
			}
			else
			{
				DOLOG("GetmoudleBaseName Failed ! " + GetLastError());
			}
			CloseHandle(hProcess);
		}
		else
		{
			DOLOG("OpenProcess Failed ! " + allProcessIDs[i] + " LastError:" + GetLastError());
		}
	}
	
	DOLOG("can't found the specify name process Failed ! ");
	return 0;
}

// [2015/12/15 wupeng] 
BOOL EnableDebugPrivilege(BOOL bEnableDebugPrivilege)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		DOLOG("OpenProcessToken Failed "+GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		DOLOG("LookupPrivilegeValue Failed "+ GetLastError());
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnableDebugPrivilege)
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		tp.Privileges[0].Attributes = 0;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		DOLOG("AdjustTokenPrivileges Failed "+ GetLastError());

		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);

	return TRUE;
}

char* GetBMPBinaryData(PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
	assert(pbi!=NULL);
	char* lpBits = (char*)malloc(pbi->bmiHeader.biSizeImage);
	assert(lpBits != NULL);
	ZeroMemory(lpBits, pbi->bmiHeader.biSizeImage);
	int res = GetDIBits(hDC, hBMP, 0, (WORD)pbi->bmiHeader.biHeight, lpBits, pbi,
		DIB_RGB_COLORS);
	assert(res != 0);
	
	return lpBits;
}
void CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi,
	HBITMAP hBMP, HDC hDC)
{
	HANDLE hf = NULL;                 // file handle  
	BITMAPFILEHEADER hdr = {0};       // bitmap file-header  
	PBITMAPINFOHEADER pbih = NULL;    // bitmap info-header  
	LPBYTE lpBits = NULL;             // memory pointer  
	DWORD dwTotal = 0;				// total count of bytes  
	DWORD cb = 0;                   // incremental count of bytes  
	BYTE *hp = 0;                   // byte pointer  
	DWORD dwTmp = 0;

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
	assert(lpBits);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	int res = GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS);
	assert(res != 0);

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	assert(hf != INVALID_HANDLE_VALUE);
	hdr.bfType = 0x4d42;        // 0x42 = "B"  0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	res = WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL);
	assert(res != 0);

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL);

	// Close the .BMP file.  
	CloseHandle(hf);

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}

void GetCurDisplay(HDC& rCompatibleHDC, HBITMAP& rHbitmap)
{
	HDC desktopDC = CreateDCW(L"DISPLAY", NULL, NULL, NULL);
	ASSERT_NOTNULL(desktopDC);

	int cx = GetDeviceCaps(desktopDC, HORZRES);
	int cy = GetDeviceCaps(desktopDC, VERTRES);

	rCompatibleHDC = CreateCompatibleDC(desktopDC);
	ASSERT_NOTNULL(rCompatibleHDC);

	rHbitmap = CreateCompatibleBitmap(desktopDC, cx, cy);
	ASSERT_NOTNULL(rHbitmap);
	HBITMAP oldbitMap = (HBITMAP)SelectObject(rCompatibleHDC, rHbitmap);

	int res = BitBlt(rCompatibleHDC, 0, 0, cx, cy, desktopDC, 0, 0, SRCCOPY);
	ASSERT_NOTZERO(res);

	rHbitmap = (HBITMAP)SelectObject(rCompatibleHDC, oldbitMap);

	DeleteDC(desktopDC);
}

void GetBitmapInfo(BITMAPINFO &rBmpinfo, HBITMAP compatibleHbitmap)
{
	BITMAP bitmap = {0};
	GetObjectA(compatibleHbitmap, sizeof(bitmap), &bitmap);

	rBmpinfo.bmiHeader.biBitCount = bitmap.bmBitsPixel;//颜色位数
	rBmpinfo.bmiHeader.biClrImportant = 0;
	rBmpinfo.bmiHeader.biCompression = BI_RGB;
	rBmpinfo.bmiHeader.biHeight = bitmap.bmHeight;
	rBmpinfo.bmiHeader.biPlanes = bitmap.bmPlanes; //级别?必须为1
	rBmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	rBmpinfo.bmiHeader.biSizeImage = bitmap.bmHeight*bitmap.bmWidth*bitmap.bmBitsPixel;
	rBmpinfo.bmiHeader.biWidth = bitmap.bmWidth;
	rBmpinfo.bmiHeader.biXPelsPerMeter = 0;
	rBmpinfo.bmiHeader.biYPelsPerMeter = 0;
}

char* GetCaptureScreenDCRGBbits(int& rWidth,int& rHeight, int& rPixelBitSize)
{
	HDC compatibleHDC = NULL;
	HBITMAP compatibleHbitmap = NULL;

	compatibleHDC = NULL;
	compatibleHbitmap = NULL;
	GetCurDisplay(compatibleHDC, compatibleHbitmap);
	ASSERT_NOTNULL(compatibleHDC);
	ASSERT_NOTNULL(compatibleHbitmap);
	BITMAPINFO rBmpinfo = { 0 };
	GetBitmapInfo(rBmpinfo, compatibleHbitmap);
	rWidth = rBmpinfo.bmiHeader.biWidth;
	rHeight = rBmpinfo.bmiHeader.biWidth;
	rPixelBitSize = rBmpinfo.bmiHeader.biBitCount;
	char* lpRGBBits = GetBMPBinaryData(&rBmpinfo, compatibleHbitmap, compatibleHDC);
	ASSERT_NOTNULL(lpRGBBits);
	DeleteDC(compatibleHDC);
	DeleteObject(compatibleHbitmap);
	return lpRGBBits;
}

void CaptureUseDC()
{
	TCHAR filename[MAX_PATH] = { 0 };
	HDC compatibleHDC = NULL;
	HBITMAP compatibleHbitmap = NULL;

	compatibleHDC = NULL;
	compatibleHbitmap = NULL;
	GetCurDisplay(compatibleHDC, compatibleHbitmap);
	BITMAPINFO rBmpinfo = { 0 };
	GetBitmapInfo(rBmpinfo, compatibleHbitmap);

	ZeroMemory(filename, sizeof(filename));
	_stprintf_s(filename, MAX_PATH, L"%d.bmp", GetTickCount());
	CreateBMPFile(filename, &rBmpinfo, compatibleHbitmap, compatibleHDC);
	DeleteDC(compatibleHDC);
	DeleteObject(compatibleHbitmap);
}

BOOL GetNtVersionNumbers32(DWORD&dwMajorVer, DWORD& dwMinorVer, DWORD& dwBuildNumber)
{
	BOOL bRet = FALSE;

	HMODULE hModNtdll = NULL;

	if (hModNtdll = ::LoadLibraryW(L"ntdll.dll"))
	{
		typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
		pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
		pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
		if (pfRtlGetNtVersionNumbers)
		{
			pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
			dwBuildNumber &= 0x0ffff;
			bRet = TRUE;
		}
		::FreeLibrary(hModNtdll);
		hModNtdll = NULL;
	}
	return bRet;
}
