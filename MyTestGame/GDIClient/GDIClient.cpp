// GDIClient.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "GDIClient.h"
#include "WpDrawObj.h"
#include <thread>

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

	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GDICLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GDICLIENT));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_GDICLIENT));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_GDICLIENT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	RECT rect = { 0, 0, MAX_X * GIRDWIDTH, MAX_Y * GIRDWIDTH };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW^WS_THICKFRAME, TRUE);

	int screencx = GetSystemMetrics(SM_CXSCREEN);
	int screency = GetSystemMetrics(SM_CYSCREEN);

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW^WS_THICKFRAME,
		(screencx - MAX_X * GIRDWIDTH) / 2, (screency - MAX_Y * GIRDWIDTH) / 2, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
static CWpDrawObj g_sDrawObj;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;

	HDC hdc;
	HDC hdcCapatible;
	HBITMAP hbitmap;

	if (g_sDrawObj.pth == NULL)
	{
		g_sDrawObj.Init(hWnd);
		static std::thread t1(CWpDrawObj::Start, &g_sDrawObj);
		g_sDrawObj.pth = &t1;
	}
	switch (message)
	{
	case WM_CREATE:
		// 创建单独线程来进行帧数循环
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			g_sDrawObj.SetOp(CObstacleArranger::GetInstance());
			break;
		case IDM_EXIT:
			g_sDrawObj.SetOp(CMoveSelector::GetInstance());
			//DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONUP:
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		g_sDrawObj.Do(pt);
		break;
	case WM_PAINT:
		// 做双缓冲
		RECT rc;
		GetClientRect(hWnd, &rc);
		hdc = BeginPaint(hWnd, &ps);
		hdcCapatible = CreateCompatibleDC(hdc);
		hbitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
		SelectObject(hdcCapatible, hbitmap);
		// 真正的绘制
		g_sDrawObj.Draw(hdcCapatible);

		BitBlt(hdc,
			0, 0,
			rc.right, rc.bottom,
			hdcCapatible,
			0, 0,
			SRCCOPY);
		DeleteObject(hbitmap);
		DeleteObject(hdcCapatible);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		g_sDrawObj.m_bExit = TRUE;
		g_sDrawObj.pth->join();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
