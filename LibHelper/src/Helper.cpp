#include "Helper.h"
#include <windows.h>
#include <Psapi.h>

#undef UNICODE
#include <Tlhelp32.h>
#undef UNICODE
#include <tchar.h>

#include <WinSock2.h>
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
		DOLOG(" CreateToolhelp32Snapshot Failed �� \n" + GetLastError());
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

	// ��Ҫ���������snapshot����  
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

	// ��Ϊ�������õ���ͼ���ǵߵ��ģ�������Ҫ��֮ǰ����ɷ���
	//int res = BitBlt(rCompatibleHDC, 0, cy, cx, -1*cy, desktopDC, 0, 0, SRCCOPY);
	/*
	StretchBltҲ����ˮƽ��ֱ��תͼ�����cxSrc��cxDst��ǣ�ת�����豸��λ�Ժ󣩲�ͬ��
	��ôStretchBlt�ͽ���һ���������ҷ�ת����STRETCH�����У�ͨ����xDst������ΪcxClient����cxDst�����ĳ�-cxClient��
	�Ϳ���������һ�㡣���cySrc��cyDst��ͬ����StretchBlt�����·�תͼ��Ҫ��STRETCH�����в�����һ�㣬
	�ɽ�yDst������ΪcyClient����cyDst�����ĳ�-cyClient��
	*/
	int res = StretchBlt(rCompatibleHDC, 0, cy, cx, -1*cy, desktopDC, 0, 0, cx, cy,SRCCOPY);
	//int res = StretchDIBits(rCompatibleHDC, 0, 0, cx, -1*cy, desktopDC, 0, 0, cx, cy,SRCCOPY);
	
	ASSERT_NOTZERO(res);

	rHbitmap = (HBITMAP)SelectObject(rCompatibleHDC, oldbitMap);

	DeleteDC(desktopDC);
}

void GetBitmapInfo(BITMAPINFO &rBmpinfo, HBITMAP compatibleHbitmap)
{
	BITMAP bitmap = {0};
	GetObjectA(compatibleHbitmap, sizeof(bitmap), &bitmap);

	rBmpinfo.bmiHeader.biBitCount = bitmap.bmBitsPixel;//��ɫλ��
	rBmpinfo.bmiHeader.biClrImportant = 0;
	rBmpinfo.bmiHeader.biCompression = BI_RGB;
	rBmpinfo.bmiHeader.biHeight = bitmap.bmHeight;
	rBmpinfo.bmiHeader.biPlanes = bitmap.bmPlanes; //����?����Ϊ1
	rBmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	rBmpinfo.bmiHeader.biSizeImage = bitmap.bmHeight*bitmap.bmWidth*bitmap.bmBitsPixel;
	rBmpinfo.bmiHeader.biWidth = bitmap.bmWidth;
	rBmpinfo.bmiHeader.biXPelsPerMeter = 0;
	rBmpinfo.bmiHeader.biYPelsPerMeter = 0;
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
		DOLOG("��process:" + processID + "ʧ��");
		return false;
	}

	DWORD needSize = 0;
	if (FALSE == EnumProcessModules(hProcess, NULL, 0, &needSize))
	{
		DOLOG("EnumProcessModules " + processID + "ʧ�� ErrCode:" + GetLastError());
		return false;
	}

	HMODULE *hMods = (HMODULE*)malloc(needSize);
	if (hMods == NULL)
	{
		DOLOG(" malloc ����");
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

unsigned long getAddrByDomain(char* pAddr)
{
	ASSERT_NOTNULLRET(pAddr, 0);
	struct hostent* ph;
	struct in_addr in;
	ph = gethostbyname(pAddr);
	if (!ph)
	{
		DOLOG("gethostbyname Error:"+WSAGetLastError());
		return 0;
	}
	memcpy(&in, ph->h_addr_list,ph->h_length);
	return in.s_addr;
}

bool InitWinsock(BYTE hi, BYTE low)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err = 0;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0){
		DOLOG("WSAStartup error");
		return false;
	}
	if (HIBYTE(wsaData.wVersion)!=low ||
		LOBYTE(wsaData.wVersion) != hi)
	{
		DOLOG("winsock version not support");
		WSACleanup();
		return false;
	}
	return true;
}

void EnableNonblock(SOCKET sock)
{
	ULONG val = 1;
	ioctlsocket(sock, FIONBIO, &val);
}

void DisableNonblock(SOCKET sock)
{
	ULONG val = 0;
	ioctlsocket(sock, FIONBIO, &val);
}

SOCKET CreateSocket(int type, int protocol)
{
	SOCKET newSock = 0;
	newSock = socket(AF_INET, type, protocol);
	if (newSock == INVALID_SOCKET)
	{
		DOLOG("create socket Error:"+ WSAGetLastError());
		return INVALID_SOCKET;
	}
	return newSock;
}


bool ConnectToServerByDomain(SOCKET sock, LPSTR lpDomain, short port)
{
	if (lpDomain == NULL)
	{
		DOLOG("IP string can't be NULL!");
		return false;
	}
	SOCKADDR_IN addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);
	addr_in.sin_addr.S_un.S_addr = getAddrByDomain(lpDomain);
	if (addr_in.sin_addr.S_un.S_addr == 0){
		DOLOG("getAddrByDomain error!");
		return false;
	}

	if (SOCKET_ERROR == connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in)))
	{
		DOLOG("connect error! code:" + WSAGetLastError());
		return false;
	}

	return true;
}

bool ConnectToServerByIP(SOCKET sock, LPSTR lpIP, unsigned short port)
{
	if (lpIP == NULL)
	{
		DOLOG("IP string can't be NULL!");
		return false;
	}
	SOCKADDR_IN addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);
	addr_in.sin_addr.S_un.S_addr = inet_addr(lpIP);
	if (SOCKET_ERROR == connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in)))
	{
		DOLOG("connect error! code:" + WSAGetLastError());
		return false;
	}

	return true;
}

bool BindServerSock(SOCKET sock, unsigned short port)
{
	SOCKADDR_IN addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port= htons(port);
	addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(sock, (SOCKADDR*)&addr_in, sizeof(addr_in))){
		DOLOG("bind socket error! code:" + WSAGetLastError());
		return false;
	}
	return true;
}

SOCKET AcceptNewConnection(SOCKET sock){
	SOCKET newSock;
	SOCKADDR_IN addr_in;
	int addrSize = sizeof(addr_in);
	if (INVALID_SOCKET == (newSock = accept(sock, (SOCKADDR*)&addr_in, &addrSize)))
	{
		DOLOG("accept error! code:" + WSAGetLastError());
		return INVALID_SOCKET;
	}
	return newSock;
}

int SendData(SOCKET sock, const char*buf, int size)
{
	int sendSize = 0;
	int code = 0;
	sendSize = send(sock, buf, size, 0);
	if (sendSize == SOCKET_ERROR){
		if ((code = WSAGetLastError()) == WSAEWOULDBLOCK){
			return 0;
		}
	}
	if ((size - sendSize)!=0)
	{
		return -1;
	}
	return sendSize;
}

int RecvData(SOCKET sock, char* recvBuf, int size)
{
	int recvSize = 0;
	recvSize = recv(sock, recvBuf, size, 0);

	// 0 ��������ֹͣ
	if (recvSize == 0){
		return SOCKET_ERROR;
	}
	if (recvSize != SOCKET_ERROR){
		return recvSize;
	}
	if (WSAGetLastError() == WSAEWOULDBLOCK){
		return 0;
	}
	return SOCKET_ERROR;
}

#define DEFAULT_LOGFILE "log.txt"
void Log(char *buf, ...)
{
	char temp_buf1[1024] = { 0 };
	char temp_buf2[1024] = { 0 };
	va_list args;
	FILE* fp;

	time_t timeVal;
	time(&timeVal);
	struct tm* currTM = localtime(&timeVal);
	fp = fopen("log.txt");
	if (!fp){
		return;
	}
	va_start(args, buf);
	vsprintf(temp_buf1, buf, args);
	va_end(args);
	sprintf(temp_buf2, "[%d/%02d/%02d %02d:%02d:%02d]%s",
		currTM->tm_year + 1900,
		currTM->tm_mon + 1,
		currTM->tm_mday,
		currTM->tm_hour,
		currTM->tm_min,
		currTM->tm_sec,
		temp_buf1
		);
	strcpy(temp_buf1, temp_buf2);
	fprintf(fp, temp_buf1);

	fclose(fp);
}

// ����close�Ƕ���û�д���������ݵķ���
bool SetCloseLinger(SOCKET sock)
{
	struct linger lingerStruct;
	lingerStruct.l_linger = 0;
	lingerStruct.l_onoff= 0;
	int res = setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct));
	if (res == SOCKET_ERROR){
		DOLOG("setsockopt SO_LINGER Error! code:"+WSAGetLastError());
		return false;
	}
	return true;
}

bool ListenSocket(SOCKET sock, int backLog)
{
	int res = listen(sock, backLog);
	if (res == SOCKET_ERROR){
		DOLOG("listen Error! code:" + WSAGetLastError());
		return false;
	}

	return true;
}