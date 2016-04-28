////////////////////////////////////////////////////
// 用来进简单测试的类
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#include "UnitTest.h"

std::list<UNITTESTPROC> UnitTest::m_TestFuncList;

UnitTest::UnitTest()
{
}

UnitTest::~UnitTest()
{

}

void UnitTest::testAll()
{
	for (std::list<UNITTESTPROC>::iterator itor = m_TestFuncList.begin();
		itor != m_TestFuncList.end(); ++itor)
	{
		(*itor)();
	}
}

// TODO: add libevent
bool UnitTest::StartSimpleLibeventServer()
{
	return true;
}

bool UnitTest::StartSimpleLibeventClient()
{
	return true;
}

// TODO: add plain socket class
bool UnitTest::StartPlainSocketServer()
{
	return true;
}

bool UnitTest::StartPlainSocketClient()
{
	return true;
}

// TODO: add plain socket select class
bool UnitTest::StartSelectSocketServer()
{
	return true;
}

bool UnitTest::StartSelectSocketClient()
{
	return true;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"ShowTextTip", L"", WS_VISIBLE,
		10,10, 100, 25, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	LONG_PTR Style = ::GetWindowLongPtr(hWnd, GWL_STYLE);
	Style = Style &~WS_CAPTION &~WS_SYSMENU &~WS_SIZEBOX;
	::SetWindowLongPtr(hWnd, GWL_STYLE, Style);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 1;
		mmi->ptMinTrackSize.y = 1;
		mmi->ptMaxTrackSize.x = 200;
		mmi->ptMaxTrackSize.y = 100;
		break;
	case WM_CREATE:
		break;
	case WM_PAINT:
		RECT rc;
		HBRUSH hbr;
		hbr = CreateSolidBrush(RGB(0, 0, 0));
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rc);
		assert(FillRect(hdc, &rc, hbr));

		SetTextColor(hdc, RGB(255, 0, 0));
		SetBkColor(hdc, RGB(0, 0, 0));
		
		DrawTextW(hdc, L"OnRecording", 11, &rc, DT_CENTER | DT_VCENTER);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"ShowTextTip";
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

bool UnitTest::ShowATopMostInfoWindow(wstring &showText)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, SW_SHOWNORMAL))
	{
		return FALSE;
	}
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}