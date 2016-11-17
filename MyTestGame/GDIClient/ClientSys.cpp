#include "stdafx.h"
#include "ClientSys.h"

ClientSys::ClientSys(ClientSysInitParam& rClientSysParam, LPWSTR lpwszTempMenu) :
m_hInst(rClientSysParam.hInstance), m_nCmdShow(rClientSysParam.nCmdShow), m_wstrTitle(rClientSysParam.wstrAppTitle),
m_wstrWindowClass(rClientSysParam.wstrWindowsClass), m_hAccelTable(rClientSysParam.hAccelTable),
m_hIcon(rClientSysParam.hIcon), m_hIconSm(rClientSysParam.hIconSm), m_ptSize(rClientSysParam.ptSize), m_lpwszTempMenu(lpwszTempMenu), m_hWndMain(NULL)
{

}

ClientSys::~ClientSys()
{
}

bool ClientSys::Initialize()
{
	MyRegisterClass();
	if (!InitInstance())
	{
		return false;
	}
	return true;
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
	wcex.hIcon = m_hIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = m_lpwszTempMenu;
	wcex.lpszClassName = m_wstrWindowClass.c_str();
	wcex.hIconSm = m_hIconSm;

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
	static ClientSys* pThis = NULL;
	if (message == WM_CREATE){
		pThis = (ClientSys*)((CREATESTRUCT*)lParam)->lpCreateParams;
	}
	if (pThis)
	{
		return pThis->MsgProc(hWnd, message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void ClientSys::OnKeyPress(WPARAM key)
{
	switch (key)
	{
	case 'r':	//开始
		break;
	case VK_SPACE:
		break;
	}
}

void ClientSys::OnCreate()
{

}

LRESULT ClientSys::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
	case WM_CREATE:
		OnCreate();
		break;
	case WM_COMMAND:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_PAINT:
		break;
	case WM_CHAR:
		OnKeyPress(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}