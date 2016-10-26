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
DWORD GetSpecificProcIDByName(LPWSTR lpName)
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

	PROCESSENTRY32W pe32;
	ZeroMemory(&pe32, sizeof(pe32));

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		DOLOG(" CreateToolhelp32Snapshot Failed ！ \n" + GetLastError());
		return 0;
	}

	BOOL bMore = Process32FirstW(hProcessSnap, &pe32);
	while (bMore)
	{
		DOLOGW(L" >>" + pe32.szExeFile + L" : " + pe32.th32ProcessID);
		if (lstrcmpW(pe32.szExeFile, lpName) == 0)
		{
			resProcessID = pe32.th32ProcessID;
		}
		bMore = Process32NextW(hProcessSnap, &pe32);
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

////////////////////////////////////////////////////
// return the color bits content of specific bitmap.
// pBuf Need height*wid*4
// wupeng
BOOL GetBMPBinaryDataEx(PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC, char* pBuf)
{
	ASSERT_NOTNULLRET(pbi, FALSE);
	ASSERT_NOTNULLRET(pBuf,  FALSE);
	int res = GetDIBits(hDC, hBMP, 0, (WORD)pbi->bmiHeader.biHeight, pBuf, pbi,
		DIB_RGB_COLORS);
	ASSERT_NOTZERORET(res, FALSE);
	return TRUE;
}

void GetBitmapInfo(BITMAPINFO &rBmpinfo, HBITMAP compatibleHbitmap)
{
	BITMAP bitmap = { 0 };
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

// 需要使用free来释放获得的BMP的内存
char* GetBMPBinaryDataEx(LPSTR pszFileName)
{
	HBITMAP hBitmap = (HBITMAP)::LoadImageA(NULL, pszFileName,
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (NULL == hBitmap)
	{
		DWORD dwError = ::GetLastError();
		return NULL;
	}
	BITMAPINFO rBmpinfo = { 0 };
	GetBitmapInfo(rBmpinfo, hBitmap);
	DeleteObject(hBitmap);
	FILE* pFile = NULL;
	errno_t errcode = fopen_s(&pFile, pszFileName, "rb");
	if (errcode != 0 && pFile == NULL){
		return NULL;
	}

	// 计算像素数据长度
	int pixelLength = rBmpinfo.bmiHeader.biWidth * 3;
	pixelLength *= rBmpinfo.bmiHeader.biHeight;

	// 读取像素数据
	char* pixelData = NULL;
	pixelData = (char*)malloc(pixelLength);
	assert(pixelData);
	if (!pixelData){
		fclose(pFile);
		return NULL;
	}
	int contentSize = sizeof(BITMAPINFOHEADER);
	contentSize += sizeof(BITMAPFILEHEADER);

	fseek(pFile, contentSize, SEEK_SET);
	fread(pixelData, pixelLength, 1, pFile);
	fclose(pFile);
	return pixelData;
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

	// 因为在正常得到的图像是颠倒的，所以需要在之前处理成反的
	//int res = BitBlt(rCompatibleHDC, 0, cy, cx, -1*cy, desktopDC, 0, 0, SRCCOPY);
	/*
	StretchBlt也允许水平或垂直翻转图像。如果cxSrc和cxDst标记（转换成设备单位以后）不同，
	那么StretchBlt就建立一个镜像：左右翻转。在STRETCH程序中，通过将xDst参数改为cxClient并将cxDst参数改成-cxClient，
	就可以做到这一点。如果cySrc和cyDst不同，则StretchBlt会上下翻转图像。要在STRETCH程序中测试这一点，
	可将yDst参数改为cyClient并将cyDst参数改成-cyClient。
	*/
	int res = StretchBlt(rCompatibleHDC, 0, cy, cx, -1*cy, desktopDC, 0, 0, cx, cy,SRCCOPY);
	//int res = StretchDIBits(rCompatibleHDC, 0, 0, cx, -1*cy, desktopDC, 0, 0, cx, cy,SRCCOPY);
	
	ASSERT_NOTZERO(res);

	rHbitmap = (HBITMAP)SelectObject(rCompatibleHDC, oldbitMap);

	DeleteDC(desktopDC);
}


	
///////////////////////////////////////////////
// pBuf size need to be width*height*4
// wupeng
BOOL GetCaptureScreenDCRGBAbitsEx(int& rWidth, int& rHeight, int& rPixelBitSize, char* pBuf)
{
	ASSERT_NOTNULLRET(pBuf, FALSE);
	HDC compatibleHDC = NULL;
	HBITMAP compatibleHbitmap = NULL;

	compatibleHDC = NULL;
	compatibleHbitmap = NULL;
	GetCurDisplay(compatibleHDC, compatibleHbitmap);
	ASSERT_NOTNULLRET(compatibleHDC, FALSE);
	ASSERT_NOTNULLRET(compatibleHbitmap, FALSE);
	BITMAPINFO rBmpinfo = { 0 };
	GetBitmapInfo(rBmpinfo, compatibleHbitmap);
	rWidth = rBmpinfo.bmiHeader.biWidth;
	rHeight = rBmpinfo.bmiHeader.biHeight;
	rPixelBitSize = rBmpinfo.bmiHeader.biBitCount;
	BOOL bRes = FALSE;
	bRes = GetBMPBinaryDataEx(&rBmpinfo, compatibleHbitmap, compatibleHDC, pBuf);
	ASSERT_NOTFALSERET(bRes, FALSE);
	DeleteDC(compatibleHDC);
	DeleteObject(compatibleHbitmap);
	return true;
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
	rHeight = rBmpinfo.bmiHeader.biHeight;
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

static wchar_t s_ItoWCacheBuff[MAX_PATH];
wchar_t* ItoWStatic(int integer)
{
	ASSERT_ZERORET(_ltow_s(integer, s_ItoWCacheBuff, MAX_PATH, 10), NULL);
	return s_ItoWCacheBuff;
}

static char s_ItoACacheBuff[MAX_PATH];
char* ItoAStatic(int integer)
{
	ASSERT_ZERORET(_ltoa_s(integer, s_ItoACacheBuff, MAX_PATH, 10), NULL);
	return s_ItoACacheBuff;
}

bool EnumSpecificProcessModule(DWORD processID, list<string>& rModuleNames)
{
	HANDLE hProcess = NULL;
	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (hProcess == NULL)
	{
		DOLOG("打开process:" + processID + "失败");
		return false;
	}

	DWORD needSize = 0;
	if (FALSE == EnumProcessModules(hProcess, NULL, 0, &needSize))
	{
		DOLOG("EnumProcessModules " + processID + "失败 ErrCode:" + GetLastError());
		return false;
	}

	HMODULE *hMods = (HMODULE*)malloc(needSize);
	if (hMods == NULL)
	{
		DOLOG(" malloc 错误");
		return false;
	}
	memset(hMods, 0, needSize);

	UINT fGraphicFeature = 0;
	if (EnumProcessModules(hProcess, hMods, needSize, &needSize))
	{
		for (unsigned int i = 0; i < (needSize / sizeof(HMODULE)); i++)
		{
			CHAR szModName[MAX_PATH] = { 0 };
			// Get the full path to the module's file.
			if (GetModuleFileNameExA(hProcess, hMods[i], szModName, (sizeof(szModName) / sizeof(TCHAR))))
			{
				rModuleNames.push_back(szModName);
				DOLOG(szModName);
			}
		}
	}
	SAFE_FREE(hMods);
	return true;
}

/////////////////////////////////////////////////////
// Enum specific process Module 
// wupeng
bool EnumSpecificProcessModuleEx(DWORD processID, list<wstring>& rModuleNames)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32W meW;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		DOLOG("CreateToolhelp32Snapshot (of modules) lastError:"+GetLastError());
		return false;
	}

	// Set the size of the structure before using it.
	meW.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &meW))
	{
		DOLOG("Module32First Failed");  // show cause of failure
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return false;
	}

	// Now walk the module list of the process,
	// and display information about each module
	do
	{
		rModuleNames.push_back(meW.szExePath);
		DOLOGW(L"\n   MODULE NAME:    = " + meW.szModule
			+ L"\n Executable     = " + meW.szExePath
			+ L"\n      Process ID     = " + meW.th32ProcessID
			+ L"\n      Ref count (g)  = " + meW.GlblcntUsage
			+ L"\n     Ref count (p)  = " + meW.ProccntUsage
			+ L"\n     Base address   = " + (DWORD)meW.modBaseAddr
			+ L"\n     Base size      = " + meW.modBaseSize);

	} while (Module32Next(hModuleSnap, &meW));

	CloseHandle(hModuleSnap);
	return true;
}

BOOL CALLBACK ProcThreadWindowsEnum(HWND hWnd, LPARAM param)
{
	RECT curRect;
	GetWindowRect(hWnd, &curRect);
	WCHAR wszClassName[MAX_PATH] = { 0 };

	ZeroMemory((void*)wszClassName, MAX_PATH * sizeof(WCHAR));
	RealGetWindowClassW(hWnd, (LPWSTR)wszClassName, MAX_PATH);
	DOLOGW(L"@@@@>> Window: " + (UINT)hWnd + L", Size:[" + curRect.left + L"," + curRect.top + L"," + curRect.right + L"," + curRect.bottom + 
		L"], ClassName:" + wszClassName+L", Style:"+ GetWindowLongA(hWnd, GWL_STYLE) + L", IsVisible:" + ((GetWindowLongA(hWnd, GWL_STYLE)&WS_VISIBLE)?L"true":L"false"));

	return TRUE;
}

BOOL EnumWindowsInSpecificProcess(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == 0)
		return FALSE;
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return FALSE;

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);    // Must clean up the
		//   snapshot object!
		return FALSE;
	}

	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			EnumThreadWindows(te32.th32ThreadID, ProcThreadWindowsEnum, NULL);
		}
	} while (Thread32Next(hThreadSnap, &te32));

	//  Don't forget to clean up the snapshot object.
	CloseHandle(hThreadSnap);
	return(TRUE);
}

void CreateBMPFileEx(LPTSTR pszFile, PBITMAPINFO pbi, LPBYTE lpBits)
{
	HANDLE hf = NULL;                 // file handle  
	BITMAPFILEHEADER hdr = { 0 };       // bitmap file-header  
	PBITMAPINFOHEADER pbih = NULL;    // bitmap info-header  
	DWORD dwTotal = 0;				// total count of bytes  
	DWORD cb = 0;                   // incremental count of bytes  
	BYTE *hp = 0;                   // byte pointer  
	DWORD dwTmp = 0;

	pbih = (PBITMAPINFOHEADER)pbi;

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
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
	BOOL res = WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
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
}

#include <sstream>

void WriteCaptureSpecificDCRGBbitsEx(LPBYTE lpBits, LPTSTR lpFilePath, UINT width, UINT height, UINT pixelBitSize)
{
	BITMAPINFO bmpinfo = { 0 };

	bmpinfo.bmiHeader.biBitCount = pixelBitSize*8;//颜色位数
	bmpinfo.bmiHeader.biClrImportant = 0;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biHeight = height;
	bmpinfo.bmiHeader.biWidth = width;
	bmpinfo.bmiHeader.biPlanes = 1; //级别?必须为1
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biSizeImage = height*width*pixelBitSize;
	bmpinfo.bmiHeader.biXPelsPerMeter = 0;
	bmpinfo.bmiHeader.biYPelsPerMeter = 0;

	CreateBMPFileEx(lpFilePath, &bmpinfo, lpBits);
}


VOID Nomalize(POINT &rPt){
	float norm = sqrt((float)((rPt.x * rPt.x) + (rPt.y * rPt.y)));
	rPt.x = (INT)ceil(rPt.x / norm);
	rPt.y = (INT)ceil(rPt.y / norm);
}

// 按照向量方向取整
float RoundDir(float pos, float dir)
{
	if (dir >= 0)
	{
		return ceil(pos);
	}
	else
	{
		return floor(pos);
	}
}

// 向量的方向走一步
int StepDir(int pos, int dir)
{
	if (dir >= 0)
	{
		pos++;
	}
	else
	{
		pos--;
	}
	return pos;
}

VOID Nomalize(FPOINT &rPt)
{
	float norm = sqrt((rPt.x * rPt.x) + (rPt.y * rPt.y));
	rPt.x = rPt.x / norm;
	rPt.y = rPt.y / norm;
}

float StepDir(float pos, float dir)
{
	if (dir >= 0)
	{
		pos++;
	}
	else
	{
		pos--;
	}
	return pos;
}

// 的量度量两点距离的标量
float Get2PointScale(FPOINT pt1, FPOINT pt2)
{
	return abs(pt1.x - pt2.x) + abs(pt1.y - pt2.y);
}

INT Get2PointScale(POINT pt1, POINT pt2)
{
	return abs(pt1.x - pt2.x) + abs(pt1.y - pt2.y);
}

INT Get2PointScaleDist(POINT pt1, POINT pt2)
{
	return ((pt1.x - pt2.x)*(pt1.x - pt2.x)) + ((pt1.y - pt2.y)*(pt1.y - pt2.y));
}