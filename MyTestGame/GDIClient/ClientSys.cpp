#include "stdafx.h"
#include "ClientSys.h"

ClientSys::ClientSys(HINSTANCE hInstance, int nCmdShow) :m_hInst(hInstance), m_nCmdShow(nCmdShow)
{
	WCHAR tempBuf[MAX_PATH] = { NULL };
	
	LoadString(m_hInst,IDS_APP_TITLE, tempBuf, MAX_PATH);
	m_wstrTitle = tempBuf;
	LoadString(m_hInst,IDC_GDICLIENT, tempBuf, MAX_PATH);
	m_wstrWindowClass = tempBuf;

	LoadString(m_hInst,IDS_MAINWNDWIDTH, tempBuf, MAX_PATH);
	m_ptSize.x = stol(tempBuf);
	LoadString(m_hInst,IDS_MAINWNDYHEIGHT, tempBuf, MAX_PATH);
	m_ptSize.y = stol(tempBuf);
	m_hAccelTable = LoadAccelerators(m_hInst, MAKEINTRESOURCE(IDC_GDICLIENT));
	m_hWndMain = NULL;
}

ClientSys::~ClientSys()
{
}

bool ClientSys::Initialize()
{
	MyRegisterClass();
	if (!InitInstance())
	{
		return FALSE;
	}
	m_hAccelTable = LoadAccelerators(m_hInst, MAKEINTRESOURCE(IDC_GDICLIENT));
}

bool ClientSys::InitInstance()
{
	RECT rect = { 0, 0, m_ptSize.x, m_ptSize.y };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW^WS_THICKFRAME, TRUE);

	int screencx = GetSystemMetrics(SM_CXSCREEN);
	int screency = GetSystemMetrics(SM_CYSCREEN);

	m_hWndMain = CreateWindowW(m_wstrWindowClass.c_str(), 
							m_wstrTitle.c_str(), 
								WS_OVERLAPPEDWINDOW^WS_THICKFRAME,
		(screencx - m_ptSize.x) / 2,
		(screency - m_ptSize.y) / 2,
		rect.right - rect.left,
		rect.bottom - rect.top, NULL, NULL, m_hInst, this);

	if (!m_hWndMain)
	{
		return false;
	}

	ShowWindow(m_hWndMain, m_nCmdShow);
	UpdateWindow(m_hWndMain);

	return true;
}

ATOM ClientSys::MyRegisterClass()
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)ClientSys::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInst;
	wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDC_GDICLIENT));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_GDICLIENT);
	wcex.lpszClassName = m_wstrWindowClass.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

int ClientSys::LoopMsg()
{
	MSG msg;
	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAcceleratorW(m_hWndMain, m_hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

LRESULT ClientSys::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static ClientSys* pThis = (ClientSys*)lParam;
	if (pThis){
		return pThis->MsgProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT ClientSys::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
	case WM_CREATE:
		break;
	case WM_COMMAND:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_PAINT:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}