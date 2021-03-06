////////////////////////////////////////////////////
// 常用帮助函数
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////
#ifndef _HELPER_H_
#define _HELPER_H_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Tlhelp32.h>
#include <sstream>
#include <string>
#include <process.h>
#include <time.h>
#include <assert.h>
#include <list>

using namespace std;

//#define POPSHOWBOX	// 是否弹框
#define USEDOLOG	// 调试信息

#define SAFE_RELEASE(ptr) if(ptr){ ptr->Release(); ptr = NULL;}
#define SAFE_DELETEEX(ptr, op) if(ptr){ op; delete ptr; ptr = NULL;}

#define SAFE_DELETEARR(ptr, op) if(ptr){ op; delete [] ptr; ptr = NULL;}
#define SAFE_FREE(ptr) if(ptr){  free(ptr); ptr = NULL;}


#define ASSERT_ZERORET(x, retval) assert(x==0); if((x)!=0) return (retval);
#define ASSERT_NOTFALSERET(x, retval) assert((x)!=FALSE); if((x)==FALSE) return (retval);
#define ASSERT_NOTNULLRET(x, retval) assert((x)!=NULL); if((x)==NULL) return (retval);
#define ASSERT_NOTZERORET(x, retval) assert((x)!=0); if((x)==0) return (retval);
#define ASSERT_TRUERET(x, retval) assert((x)==TRUE); if((x)!=TRUE) return (retval);
#define ASSERT_FALSERET(x, retval) assert((x)==FALSE); if((x)!=FALSE) return (retval);
#define ASSERT_EQURET(x, val, retval) assert((x)==(val)); if((x)!=(val)) return (retval);
#define ASSERT_NEQURET(x, val, retval) assert((x)!=(val)); if((x)==(val)) return (retval);
#define ASSERT_BETWEEN(valgtr, x, valls, retval) assert((valgtr)>(x) && (x)>(valls)); if((x)==(val)) return (retval);


#define ASSERT_EQU(x, val) assert((x)==(val));
#define ASSERT_NEQU(x, val) assert((x)!=(val));
#define ASSERT_NOTNULL(x) assert((x)!=NULL);
#define ASSERT_NOTZERO(x) assert((x)!=0);
#define ASSERT_TRUE(x) assert((x)==TRUE);
#define ASSERT_FALSE(x) assert((x)==FALSE);

// [2015/12/11 wupeng] 
// Convinient to cat string
class SstringEx
{
public:
	SstringEx& operator+(const int &v){
		m_ss << v;
		return *this;
	}

	SstringEx& operator+(char* v){
		m_ss << v;
		return *this;
	}

	SstringEx& operator+(const char* v){
		m_ss << v;
		return *this;
	}

	SstringEx& operator+(const string& v){
		m_ss << v.c_str();
		return *this;
	}

	SstringEx& operator+(double &v){
		m_ss << v;
		return *this;
	}
public:
	std::stringstream m_ss;
};

class WSstringEx
{
public:
	WSstringEx& operator+(const int &v){
		m_ss << v;
		return *this;
	}

	WSstringEx& operator+(wchar_t* v){
		m_ss << v;
		return *this;
	}
	WSstringEx& operator+(const wchar_t* v){
		m_ss << v;
		return *this;
	}
	WSstringEx& operator+(const wstring& v){
		m_ss << v.c_str();
		return *this;
	}

	WSstringEx& operator+(double &v){
		m_ss << v;
		return *this;
	}
public:
	std::wstringstream m_ss;
};

// [2015/12/11 wupeng] 
// Debug log help macro
#ifdef USEDOLOG
#ifdef POPSHOWBOX
	#define DOLOGS(msg) {		\
		SstringEx tempMsg;		\
		MessageBoxA(NULL, (tempMsg + msg).m_ss.str().c_str(), "Infomation", MB_OK); \
		}

	#define DOLOG(msg) {		\
		SstringEx tempMsg;		\
		MessageBoxA(NULL, (tempMsg +"wupeng::" + msg + " (" + __FILE__ + ":" + __LINE__ + ")\n").m_ss.str().c_str(), "Infomation", MB_OK); \
	}
#else
	#define DOLOGS(msg) {	\
		SstringEx tempMsg;	\
		OutputDebugStringA((tempMsg + msg).m_ss.str().c_str()); \
	}

	#define DOLOG(msg) {	\
		SstringEx tempMsg;	\
		OutputDebugStringA((tempMsg + "wupeng::" + msg + " (" + __FILE__ + ":" + __LINE__ + ")\n").m_ss.str().c_str()); \
	}
#endif

#define COUNTTAB(x) {\
for (int i = 0; i < x; ++i){	\
	DOLOGS("\t");\
}\
}


#define W2(x) L ## x
#define W(x) W2(x)

#ifdef POPSHOWBOX
#define DOLOGW(msg) {		\
		WSstringEx tempMsg;		\
		MessageBoxA(NULL, (tempMsg +L"wupeng::" + msg + L" (" + W(__FILE__) + L":" + __LINE__ + L")\n").m_ss.str().c_str(), "Infomation", MB_OK); \
		}
#else
#define DOLOGW(msg) {	\
		WSstringEx tempMsg;	\
		OutputDebugStringW((tempMsg + L"wupeng::" + msg + L" (" + W(__FILE__) + L":" + __LINE__ + L")\n").m_ss.str().c_str()); \
	}
#endif

#else // USEDOLOG
	#define DOLOG(msg)
#endif // USEDOLOG

#define SAFE_DELETE(ptr) if(ptr != NULL){delete ptr; ptr = NULL; }

#define NULL_RET(exp, msg, ret) if(NULL == (exp)){ DOLOG(msg); ret; }
#define FALSE_RET(exp, msg, ret) if(FALSE == (exp)){ DOLOG(msg); ret; }
#define TRUE_RET(exp, msg, ret) if(TRUE == (exp)){ DOLOG(msg); ret; }

#define ISFALSE(x) ((x) == FALSE)
#define ISNOTTRUE(x) ((x) != TRUE)
#define ISTRUE(x) ((x) == TRUE)
#define ISZERO(x) ((x) == 0)
#define ISNOZERO(x) ((x) != 0)
#define ISNULL(x) ((x) == NULL)
#define ISNOTNULL(x) ((x) != NULL)

DWORD GetSpecificProcIDByName(LPWSTR lpName);
DWORD GetSpecificProcIDByNameEx(LPSTR lpName);
BOOL EnableDebugPrivilege(BOOL bEnableDebugPrivilege);

#include <string>
#include <process.h>
#include <time.h>
using namespace std;

#define ISINRANGE(var, minor, greator) (( (var) >= (minor) && (var) <= (greator) )? true : false)

#define CHECKNULLALARAM1_RET(val, msg, retstm) if( NULL == (val)){ DOERR(msg); retstm; }
#define CHECKNULL1_RET(val, retstm) if(NULL == (val)){ DOERR(msg); retstm; }

////////////////////////////////////////////////////
// 字符串帮助函数
// Wupeng 
////////////////////////////////////////////////////
const int kConvertCodepageCacheLen = 2048;
///////////////////////////////////////////////////
// 需要使用free来释放所得到的字符串内存
BOOL ConvertWidechar2ANSIMalloc(const wchar_t* pwStr, char** pStr);

// Windows可用
BOOL ConvertWidechar2UTF8(const wstring &rWStr, string &rStr);
BOOL ConvertWidechar2ANSI(const wstring &rWStr, string &rStr);
BOOL ConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL ConvertANSI2Widechar(const string& rStr, wstring& rWStr);
// CRT 跨平台，如果是linux下列都可用.
// 如果是windows，用上面的函数。winCRT没有实现UTF8
BOOL CRTConvertANSI2Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertWidechar2ANSI(const wstring& rWStr, string& rStr);
BOOL CRTConvertWidechar2UTF8(const wstring& rWStr, string& rStr);

// 适用于从保存有little ending的unicode string 内容拷贝到真正的wstring中
BOOL CopyStringToWString(wstring &targetWStr, string &srcStr);

////////////////////////////////////////////////////
// 判断是否是小端序
// Wupeng 
////////////////////////////////////////////////////
bool IsByteLittleEndian(bool &bIsLittleEndian);

//big endian conversion functions
// net byte sequence is bigendian
#define ULONGLONG_BE(val) (((val>>56)&0xFF) | (((val>>48)&0xFF)<<8) | (((val>>40)&0xFF)<<16) | (((val>>32)&0xFF)<<24) | \
	(((val >> 24) & 0xFF) << 32) | (((val >> 16) & 0xFF) << 40) | (((val >> 8) & 0xFF) << 48) | ((val & 0xFF) << 56))
#define DWORD_BE(val) (((val>>24)&0xFF) | (((val>>16)&0xFF)<<8) | (((val>>8)&0xFF)<<16) | ((val&0xFF)<<24))
#define WORD_BE(val)  (((val>>8)&0xFF) | ((val&0xFF)<<8))

#define DWORD_LE(val) (((val<<24)&0xFF) | (((val<<16)&0xFF)>>8) | (((val<<8)&0xFF)>>16) | ((val&0xFF)>>24))
#define WORD_LE(val)  (((val<<8)&0xFF) | ((val&0xFF)>>8))

__forceinline ULONGLONG fastHtonll(ULONGLONG qw)
{
	return ULONGLONG_BE(qw);
}
__forceinline DWORD fastHtonl(DWORD dw)
{
	return DWORD_BE(dw);
}
__forceinline  WORD fastHtons(WORD  w)
{
	return  WORD_BE(w);
}

////////////////////////////////////////////////////
// 得到当前执行程序的路径
// Wupeng 
////////////////////////////////////////////////////
bool GetAppPathA(string& strPath);
bool GetAppPathW(wstring& strPath);
//只得到APP的名字
bool GetAppNameA(string& strPath);
bool GetAppNameW(wstring& strPath);

bool TrimFilePathA(string& strPath);
bool TrimFilePathW(wstring& strPath);

////////////////////////////////////////////////////
// 替换所有匹配的字符串
// Wupeng 
////////////////////////////////////////////////////
string &ReplaceAll(string& rSrcString, string& rSearchString, string& rReplaceString);

////////////////////////////////////////////////////
// 使用原子操作来实现锁
// Wupeng 
////////////////////////////////////////////////////
class LiteUnpendLock
{
public:
	LiteUnpendLock() : m_lock(0) {}
	~LiteUnpendLock(){}

	bool Lock()
	{
		return InterlockedExchange(&m_lock, 1) == 0;
	}

	void Unlock()
	{
		InterlockedExchange(&m_lock, 0);
	}

private:
	volatile unsigned int m_lock;
};

class LiteUnpendLockEx
{
public:

	LiteUnpendLockEx() : m_lock(0) {}
	~LiteUnpendLockEx(){}

	bool GetLock()
	{
		return InterlockedCompareExchange(&m_lock, 1, 0) == 0;
	}

	void Unlock()
	{
		InterlockedExchange(&m_lock, 0);
	}

private:
	volatile unsigned int m_lock;
};

////////////////////////////////////////////////////
// 线程类
// Wupeng 
////////////////////////////////////////////////////
template<typename T>
class LiteThread
{
public:
	LiteThread(T *pInst);
	~LiteThread();

	BOOL close();
	BOOL start();

public:
	T *m_pInst;
	HANDLE m_hThread;
	unsigned int m_threadID;
};

template<typename T>
LiteThread<T>::LiteThread(T *pInst)
{
	m_pInst = pInst;
	m_hThread = 0;
	m_threadID = 0;
}

template<typename T>
LiteThread<T>::~LiteThread()
{
	m_pInst = NULL;
	m_hThread = 0;
	m_threadID = 0;
}

template<typename T>
BOOL LiteThread<T>::close()
{
	if (!m_hThread)
	{
		return TRUE;
	}
	if (0 == WaitForSingleObject(m_hThread, 0))
	{
		if (CloseHandle(m_hThread))
		{
			m_pInst = NULL;
			m_hThread = 0;
			m_threadID = 0;
			return TRUE;
		}
	}
	return FALSE;
}

template<typename T>
BOOL LiteThread<T>::start()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, T::RunFunc, (T*)this, 0, &m_threadID);
	if (0 == m_hThread)
	{
		return FALSE;
	}
	return TRUE;
}

template<typename T>
class CLiteSingleton{
public:
	static T& Instance(){
		static T s_Inst;
		return s_Inst;
	}
};

////////////////////////////////////////////////////
// 时间管理
// Wupeng 
////////////////////////////////////////////////////
class TimeManager
{
public:
	// 得到未来时间
	static time_t GetOverDayTime(time_t curTime);
	static time_t GetOverDayTime();
	static void PrintTime(time_t atime);
	static void GetCurTimeStr(string& strTime, time_t atime);
	static void CurrentDateTimeString(string& strTime);
	static void CurrentTimeString(string& strTime);
};


// 在指定位置插入字符串
// [2015/12/17 wupeng] 
BOOL InsertStrInStrSpecifyPosA(LPSTR lpTarBuf, UINT cBufLen, LPSTR lpSrcStr, LPSTR lpInsertStr, UINT iPos);

////////////////////////////////////////////////////
// 将当前显示器截图保存到当前目录
// wupeng 
////////////////////////////////////////////////////
void CaptureUseDC();

////////////////////////////////////////////////////
// 得到当前Screen的RGB内容
// wupeng 
// return 需要使用free来释放
char* GetCaptureScreenDCRGBbits(int& rWidth, int& rHeight, int& rPixelBitSize);

////////////////////////////////////////////////////
// 得到当前桌面的RGBA的内容, pBuf的大小应该是width*height*4
// wupeng 
BOOL GetCaptureScreenDCRGBAbitsEx(int& rWidth, int& rHeight, int& rPixelBitSize, char* pBuf);

////////////////////////////////////////////////////
// 不依赖于menifest. 得到真正操作系统的版本号，
// wupeng 
// return 需要使用free来释放
BOOL GetNtVersionNumbers32(DWORD& dwMajorVer, DWORD& dwMinorVer, DWORD& dwBuildNumber);

////////////////////////////////////////////////////
// 得到integer对应的字符串,不可重入
// wupeng 
// return 不需要释放
wchar_t* ItoWStatic(int integer);
char* ItoAStatic(int integer);

/////////////////////////////////////////////////////
// use count time pass by high time resolution
// wupeng
class HighQualityResolutionTimeLite
{
public:
	HighQualityResolutionTimeLite(){
		m_StartingTime.QuadPart = 0;
		QueryPerformanceFrequency(&m_Frequency);
	}
	void Reset(){
		QueryPerformanceCounter(&m_StartingTime);
	}

	// Unit is micro second
	LONGLONG GetTimelapse(){
		LARGE_INTEGER performanceCount;
		QueryPerformanceCounter(&performanceCount);
		performanceCount.QuadPart = performanceCount.QuadPart - m_StartingTime.QuadPart;
		return (performanceCount.QuadPart * 1000000 / m_Frequency.QuadPart);
	}
private:	
	LARGE_INTEGER m_StartingTime;
	LARGE_INTEGER m_Frequency;
};

/////////////////////////////////////////////////////
// Enum specific process Module, 不能够遍历64位，而且需要Debug权限。
// wupeng
bool EnumSpecificProcessModule(DWORD processID, list<string>& pModuleNames);

/////////////////////////////////////////////////////
// Enum specific process Module 
// wupeng
bool EnumSpecificProcessModuleEx(DWORD processID, list<wstring>& pModuleNames);

/////////////////////////////////////////////////////
// Enum windows in specific process 
// wupeng
BOOL EnumWindowsInSpecificProcess(DWORD dwOwnerPID);

void CreateBMPFileEx(LPTSTR pszFile, PBITMAPINFO pbi, LPBYTE lpBits);
void WriteCaptureSpecificDCRGBbitsEx(LPBYTE lpBits, LPTSTR lpFilePath, UINT width, UINT height, UINT pixelBitSize);

/**
 * Get File Size
 */
bool GetFileSizeW(wstring& filePath, DWORD &fileSize);


// 浮点数点
class FPOINT
{
public:
	float x;
	float y;
	FPOINT() :x(0), y(0){ }
	FPOINT(float fx, float fy) :x(fx), y(fy){ }
	FPOINT(int ix, int iy) :x((float)x), y((float)y){ }
	FPOINT(POINT pt) :x((float)pt.x), y((float)pt.y){ }
	FPOINT operator-(FPOINT& fpt)
	{
		FPOINT p;
		p.x = x - fpt.x;
		p.y = y - fpt.y;
		return p;
	}

	FPOINT operator+(FPOINT& fpt)
	{
		FPOINT p;
		p.x = x + fpt.x;
		p.y = y + fpt.y;
		return p;
	}

	FPOINT operator*(float p)
	{
		FPOINT pt;
		pt.x = x*p;
		pt.y = y*p;
		return pt;
	}

	void FillPt(POINT &rPt)
	{
		rPt.x = (INT)x;
		rPt.y = (INT)y;
	}
};

VOID Nomalize(POINT &rPt);					// 一般化
VOID Nomalize(FPOINT &rPt);					// 一般化
float RoundDir(float pos, float dir);		// 按照向量方向取整
int StepDir(int pos, int dir);				// 向量的方向走一步

VOID Nomalize(FPOINT &rPt);					// 一般化
float StepDir(float pos, float dir);		// 向量的方向走一步
float Get2PointScale(FPOINT pt1, FPOINT pt2);
INT Get2PointScale(POINT pt1, POINT pt2);
INT Get2PointScaleDist(POINT pt1, POINT pt2);

#endif //HELPER_H_