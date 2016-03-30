
#pragma once
#define _WIN32_WINNT 0x0500 
#include"windows.h"
#include"tlhelp32.h"
#include"stdio.h"
#include"NativeApi.h"
#include"wchar.h"
#include"psapi.h"//SDK6.0
#pragma comment(lib,"psapi.lib")////SDK6.0,不知道为什么vc6好像没有自带这个头文件？？

int GetUserPath(WCHAR* szModPath);
BOOL GetProcessModule(DWORD dwPID)
{
    BOOL bRet    =    FALSE;
    BOOL bFound    =    FALSE;
    HANDLE hModuleSnap = NULL;
    MODULEENTRY32 me32 ={0};
	
    hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPID);//创建进程快照
    if(hModuleSnap == INVALID_HANDLE_VALUE)
	{   
		printf("获取模块失败!\n");
		return FALSE;
	}
	
    me32.dwSize = sizeof(MODULEENTRY32);
    if(::Module32First(hModuleSnap,&me32))//获得第一个模块
	{
		do{
			printf("方法1列模块名：%s\n",me32.szExePath);
		}while(::Module32Next(hModuleSnap,&me32));
	}//递归枚举模块
	
	CloseHandle(hModuleSnap);
	return bFound;
}

bool ForceLookUpModule(DWORD dwPID)
{
	typedef DWORD( WINAPI *FunLookModule)(
		HANDLE ProcessHandle,
		DWORD BaseAddress,
		DWORD MemoryInformationClass,
		DWORD MemoryInformation,
		DWORD MemoryInformationLength,
		DWORD ReturnLength );
	HMODULE hModule = GetModuleHandle ("ntdll.dll" ) ;
	if(hModule==NULL)
	{ 
		return FALSE;
	}
    FunLookModule ZwQueryVirtualMemory=(FunLookModule)GetProcAddress(hModule,"ZwQueryVirtualMemory");
	if(ZwQueryVirtualMemory==NULL)
	{
		return FALSE;
	}
	HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION,1,dwPID);
	if(hProcess==NULL)
		return FALSE;
	PMEMORY_SECTION_NAME Out_Data=(PMEMORY_SECTION_NAME)	malloc(0x200u);
	DWORD retLength;
	WCHAR Path[256]={0};
	wchar_t wstr[256]={0};
	
	for(unsigned int i=0;i<0x7fffffff;i=i+0x10000)
	{ 
		if( ZwQueryVirtualMemory(hProcess,(DWORD)i,2,(DWORD)Out_Data,512,(DWORD)&retLength)>0)
		{ 
			if(!IsBadReadPtr((BYTE*)Out_Data->SectionFileName.Buffer,1))
			{
				if(((BYTE*)Out_Data->SectionFileName.Buffer)[0]==0x5c)
				{
					if(wcscmp(wstr, Out_Data->SectionFileName.Buffer))			
					{   
						_wsetlocale(0,L"chs"); 				
						GetUserPath(Out_Data->SectionFileName.Buffer);
						wprintf(L"方法2列模块%s\n",Out_Data->SectionFileName.Buffer);
					}
					wcscpy(wstr,   Out_Data->SectionFileName.Buffer);
				}	
			}
		}
	}
	CloseHandle(hProcess);
	return TRUE;
	
}
int GetUserPath(WCHAR* szModPath)
{    //\Device\HarddiskVolume1, 
	WCHAR Path[256]={0};
	WCHAR* Temp3=new WCHAR[3];	
	Temp3[2]='\0';	
	Temp3[1]=':';
	THead* phead=new THead;
	phead->Next=NULL;
	phead->Num=szModPath[22];
	for(int i='C';i<='Z';i++)
	{Temp3[0]=i;
	if(QueryDosDeviceW(Temp3,Path,30))
		if(phead->Num==Path[22])
		{  
			phead->Disk=(WCHAR)i;
			break;
		}
	}
	   
   szModPath[0]=phead->Disk;
   szModPath[1]=':';
   szModPath[2]='\0';
   wcscpy(Path,szModPath+23);
   wcscat(szModPath,Path);
   
   delete phead;
   delete Temp3; 
   
   return 0;
}
BOOL EnableDebugPrivilege(BOOL fEnable)//这个用于提权的
{  
	BOOL fOk = FALSE;   
	HANDLE hToken;
	
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES,&hToken))
	{   
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	else
	{
		return 0;
	}
	return(fOk);
}

void EnumModlueAll(DWORD dwPID)
{   
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,false,dwPID);
	if(hProcess==INVALID_HANDLE_VALUE)
	{ printf(" open process failed!\n");
	return;
	}
	DWORD size=0,ret=0;
	EnumProcessModules(hProcess,NULL,size,&ret);
	HMODULE *parry=(HMODULE*)malloc(ret+4);
	memset(parry,0,ret+4);
	if(EnumProcessModules(hProcess,parry,ret+4,&ret))
	{
		char* path=new char[MAX_PATH];
		memset(path,0,MAX_PATH);
		UINT i=0;
		
		while(GetModuleFileNameEx(hProcess,parry[i],path,MAX_PATH))
		{
			printf("方法3模块：%s\n",path);
			memset(path,0,MAX_PATH);
			i++;
		}
		delete path;
		
	}
	free(parry);
	
	CloseHandle(hProcess);
}

void EnumModuleEx(DWORD dwPID)
{   
	DWORD status;
	HMODULE hMod=GetModuleHandle("ntdll.dll");
	RTLCREATEQUERYDEBUGBUFFER RtlCreateQueryDebugBuffer=(RTLCREATEQUERYDEBUGBUFFER )GetProcAddress(hMod,"RtlCreateQueryDebugBuffer");
	RTLQUERYPROCESSDEBUGINFORMATION RtlQueryProcessDebugInformation=(RTLQUERYPROCESSDEBUGINFORMATION)GetProcAddress(hMod,"RtlQueryProcessDebugInformation");
	RTLDESTROYDEBUGBUFFER RtlDestroyQueryDebugBuffer =(RTLDESTROYDEBUGBUFFER )GetProcAddress(hMod,"RtlDestroyQueryDebugBuffer");
	if((hMod==NULL)||(RtlDestroyQueryDebugBuffer==NULL)||(RtlQueryProcessDebugInformation==NULL)||(RtlCreateQueryDebugBuffer==NULL))
	{
		printf("函数定位失败！\n");
		return ;
	}	
	
	PDEBUG_BUFFER Buffer=RtlCreateQueryDebugBuffer(0,FALSE);
	status=RtlQueryProcessDebugInformation(dwPID,PDI_MODULES ,Buffer);
	if(status<0)
	{ 
		printf("RtlQueryProcessDebugInformation函数调用失败，进程开了保护\n");
		return ;
	}
	ULONG count=*(PULONG)(Buffer->ModuleInformation);
	ULONG hModule=NULL;
	PDEBUG_MODULE_INFORMATION ModuleInfo=(PDEBUG_MODULE_INFORMATION)((ULONG)Buffer->ModuleInformation+4);
	for(ULONG i=0;i<count;i++)
	{
		printf("方法4列出的模块：%s\n",ModuleInfo->ImageName);
		ModuleInfo++;
	}
	
	RtlDestroyQueryDebugBuffer(Buffer);
}

void EnumSelfModule()
{
	void *PEB         = NULL,
		*Ldr         = NULL,
		*Flink       = NULL,
		*p           = NULL,
		*BaseAddress = NULL,
		*FullDllName = NULL;
	printf("列举自身模块！\n");
	__asm
	{
		mov     eax,fs:[0x30]
			mov     PEB,eax
	}
	printf( "PEB   = 0x%08X\n", PEB );
	Ldr   = *( ( void ** )( ( unsigned char * )PEB + 0x0c ) );
	printf( "Ldr   = 0x%08X\n", Ldr );
	Flink = *( ( void ** )( ( unsigned char * )Ldr + 0x0c ) );
	printf( "Flink = 0x%08X\n", Flink );
	p     = Flink;
	do
	{
		BaseAddress = *( ( void ** )( ( unsigned char * )p + 0x18 ) );
		FullDllName = *( ( void ** )( ( unsigned char * )p + 0x28 ) );
		printf( "p     = 0x%08X 0x%08X ", p, BaseAddress );
		wprintf( L"%s\n", FullDllName );
		p = *( ( void ** )p );
	}
	while ( Flink != p );
	return;
	
}

#define PAGE_SIZE 0x1000
void  Search();
bool IsValidModule(ULONG i);
bool PrintModule();
void main();
bool IsValidModule(byte* i)
{   
	if(IsBadReadPtr((void*)i,sizeof(IMAGE_DOS_HEADER)))
		return false;
	IMAGE_DOS_HEADER *BasePoint=(IMAGE_DOS_HEADER *)i;
	PIMAGE_NT_HEADERS32 NtHead=(PIMAGE_NT_HEADERS32)(i+BasePoint->e_lfanew);
	if(IsBadReadPtr((void*)NtHead,PAGE_SIZE))
		return false;
	if((NtHead->FileHeader.Characteristics&IMAGE_FILE_DLL)==0)//过滤掉。exe文件
		return false;
	if(NtHead->OptionalHeader.Subsystem==0x2)
		return true;
	if(NtHead->OptionalHeader.Subsystem==0x3)
		return true;
	return false;
}

void Search()
{   
	printf("暴力搜索列举模块!\n");
	UCHAR* i=(PUCHAR)0x10000000;
	int Num=0;
	for(;i<(PUCHAR)0x7ffeffff;i+=PAGE_SIZE)
	{   
		if(IsValidModule(i))
		{
			printf("\t\t find a module at %08x\n",i);
			Num++;
		}	
		
	}
	printf("\t\t total find module :%03d\n",Num);
}

void main()
{
	EnableDebugPrivilege(true);
	EnumModlueAll(4228);
	ForceLookUpModule(4228);
	getchar();
	GetProcessModule(4228);
	EnumModuleEx(4228);
	getchar();
	EnumSelfModule();
	getchar();
	Search();
	printf("按任意键退出........");
	getchar();
}