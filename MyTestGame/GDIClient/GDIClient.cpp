// GDIClient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GDIClient.h"
#include "WpDrawObj.h"
#include <thread>
#include "ClientSys.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WCHAR tempBuf[MAX_PATH] = { NULL };

	ClientSys::ClientSysInitParam paramInit;
	paramInit.hInstance = hInstance;
	paramInit.nCmdShow = nCmdShow;
	LoadString(hInstance, IDS_APP_TITLE, tempBuf, MAX_PATH);
	paramInit.wstrAppTitle = tempBuf;
	LoadString(hInstance, IDC_GDICLIENT, tempBuf, MAX_PATH);
	paramInit.wstrWindowsClass = tempBuf;
	paramInit.ptSize.x = 800;
	paramInit.ptSize.y = 600;
	paramInit.hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GDICLIENT));
	paramInit.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_GDICLIENT));
	paramInit.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	ClientSys client(paramInit, MAKEINTRESOURCE(IDC_GDICLIENT));
	client.Initialize();
	return client.LoopMsg();
}