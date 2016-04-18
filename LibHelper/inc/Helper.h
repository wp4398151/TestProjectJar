////////////////////////////////////////////////////
// ���ð�������
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////
#ifndef _HELPER_H_
#define _HELPER_H_

#include <Windows.h>
#include <sstream>
#include <string>
#include <process.h>
#include <time.h>
#include <assert.h>

using namespace std;

//#define POPSHOWBOX	// �Ƿ񵯿�
#define USEDOLOG	// ������Ϣ

#define SAFE_RELEASE(ptr) if(ptr){ ptr->Release(); ptr = NULL;}
#define SAFE_DELETEEX(ptr, op) if(ptr){ op; delete ptr; ptr = NULL;}
#define SAFE_FREE(ptr) if(ptr){  free(ptr); ptr = NULL;}


#define ASSERT_ZERORET(x, retval) assert(x==0); if((x)!=0) return (retval);
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

	SstringEx& operator+(double &v){
		m_ss << v;
		return *this;
	}
public:
	std::stringstream m_ss;
};

// [2015/12/11 wupeng] 
// Debug log help macro
#ifdef USEDOLOG
#ifdef POPSHOWBOX
	#define DOLOG(msg) {		\
		SstringEx tempMsg;		\
		MessageBoxA(NULL, (tempMsg +"wupeng::" + msg + " (" + __FILE__ + ":" + __LINE__ + ")\n").m_ss.str().c_str(), "Infomation", MB_OK); \
	}
#else
	#define DOLOG(msg) {	\
		SstringEx tempMsg;	\
		OutputDebugStringA((tempMsg + "wupeng::" + msg + " (" + __FILE__ + ":" + __LINE__ + ")\n").m_ss.str().c_str()); \
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

DWORD GetSpecificProcIDByName(LPSTR lpName);
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
// �ַ�����������
// Wupeng 
////////////////////////////////////////////////////
const int kConvertCodepageCacheLen = 2048;
///////////////////////////////////////////////////
// ��Ҫʹ��free���ͷ����õ����ַ����ڴ�
BOOL ConvertWidechar2ANSIMalloc(const wchar_t* pwStr, char** pStr);

// Windows����
BOOL ConvertWidechar2UTF8(const wstring &rWStr, string &rStr);
BOOL ConvertWidechar2ANSI(const wstring &rWStr, string &rStr);
BOOL ConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL ConvertANSI2Widechar(const string& rStr, wstring& rWStr);
// CRT ��ƽ̨�������linux���ж�����.
// �����windows��������ĺ�����winCRTû��ʵ��UTF8
BOOL CRTConvertANSI2Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertUTF82Widechar(const string& rStr, wstring& rWStr);
BOOL CRTConvertWidechar2ANSI(const wstring& rWStr, string& rStr);
BOOL CRTConvertWidechar2UTF8(const wstring& rWStr, string& rStr);

// �����ڴӱ�����little ending��unicode string ���ݿ�����������wstring��
BOOL CopyStringToWString(wstring &targetWStr, string &srcStr);

////////////////////////////////////////////////////
// �ж��Ƿ���С����
// Wupeng 
////////////////////////////////////////////////////
bool IsByteLittleEndian(bool &bIsLittleEndian);

////////////////////////////////////////////////////
// �õ���ǰִ�г����·��
// Wupeng 
////////////////////////////////////////////////////
bool GetAppPathA(string& strPath);
bool GetAppPathW(wstring& strPath);
//ֻ�õ�APP������
bool GetAppNameA(string& strPath);
bool GetAppNameW(wstring& strPath);



////////////////////////////////////////////////////
// �滻����ƥ����ַ���
// Wupeng 
////////////////////////////////////////////////////
string &ReplaceAll(string& rSrcString, string& rSearchString, string& rReplaceString);

////////////////////////////////////////////////////
// ʹ��ԭ�Ӳ�����ʵ����
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
// �߳���
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

protected:
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
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, T::RunFunc, m_pInst, 0, &m_threadID);
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
// ʱ�����
// Wupeng 
////////////////////////////////////////////////////
class TimeManager
{
public:
	// �õ�δ��ʱ��
	static time_t GetOverDayTime(time_t curTime);
	static time_t GetOverDayTime();
	static void PrintTime(time_t atime);
};

// ��ָ��λ�ò����ַ���
// [2015/12/17 wupeng] 
BOOL InsertStrInStrSpecifyPosA(LPSTR lpTarBuf, UINT cBufLen, LPSTR lpSrcStr, LPSTR lpInsertStr, UINT iPos);

////////////////////////////////////////////////////
// ����ǰ��ʾ����ͼ���浽��ǰĿ¼
// wupeng 
////////////////////////////////////////////////////
void CaptureUseDC();

////////////////////////////////////////////////////
// �õ���ǰScreen��RGB����
// wupeng 
// return ��Ҫʹ��free���ͷ�
char* GetCaptureScreenDCRGBbits(int& rWidth, int& rHeight, int& rPixelBitSize);

////////////////////////////////////////////////////
// ��������menifest. �õ���������ϵͳ�İ汾�ţ�
// wupeng 
// return ��Ҫʹ��free���ͷ�
BOOL GetNtVersionNumbers32(DWORD& dwMajorVer, DWORD& dwMinorVer, DWORD& dwBuildNumber);

////////////////////////////////////////////////////
// �õ�integer��Ӧ���ַ���,��������
// wupeng 
// return ����Ҫ�ͷ�
wchar_t* ItoWStatic(int integer);
char* ItoAStatic(int integer);

#endif //HELPER_H_