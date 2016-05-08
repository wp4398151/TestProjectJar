////////////////////////////////////////////////////
// Show Tip On The Top Most Desktop.
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#include "TipMsgWindow.h"
#include <assert.h>
#include "Helper.h"

bool TipMsgWindow::Init(LPSTR lpTextMsg, POINT& pos, POINT& size)
{
	assert(lpTextMsg);
	strcpy_s(m_Msg, MAX_PATH, lpTextMsg);
	m_pos = pos;
	m_size = size;
	return true;
}

BOOL TipMsgWindow::InitInstance(HINSTANCE hInstance)
{
	HWND hWnd;
	hWnd = CreateWindowExW(WS_EX_TOOLWINDOW |WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"ShowTextTip", L"", WS_VISIBLE,
		m_pos.x, m_pos.y, m_size.x, m_size.y, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		DOLOG("CreateWindowExW Failed! ErrorCode:" + GetLastError());
		return FALSE;
	}
	SetWindowLong(hWnd, GWL_USERDATA, (LONG)this);
	ShowWindow(hWnd, SW_SHOWNORMAL);
	LONG_PTR Style = ::GetWindowLongPtr(hWnd, GWL_STYLE);
	Style = Style &~WS_CAPTION &~WS_SYSMENU &~WS_SIZEBOX;
	::SetWindowLongPtr(hWnd, GWL_STYLE, Style);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK TipMsgWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TipMsgWindow *pThis = NULL;

	switch (message)
	{
	case WM_TIMER:
		BringWindowToTop(hWnd);
		break;
	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 1;
		mmi->ptMinTrackSize.y = 1;
		mmi->ptMaxTrackSize.x = 200;
		mmi->ptMaxTrackSize.y = 200;
		break;
	case WM_CREATE:
		SetTimer(hWnd,0, 1000, NULL);
		break;
	case WM_PAINT:
		RECT rc;
		pThis = (TipMsgWindow*)GetWindowLong(hWnd, GWL_USERDATA);
		if (pThis == NULL)
		{
			DOLOG("have not right TipMsgWindow instance£¡");
			break;
		}
		
		hdc = BeginPaint(hWnd, &ps);
		//hbr = CreateSolidBrush(RGB(0, 0, 0));
		rc.top = 0;
		rc.left = 0;
		rc.right = pThis->m_size.x;
		rc.bottom = pThis->m_size.y;
		//assert(FillRect(hdc, &rc, hbr));
		SetTextColor(hdc, RGB(255, 0, 0));
		SetBkColor(hdc, RGB(0, 0, 0));
		DrawTextA(hdc, pThis->m_Msg, strlen(pThis->m_Msg), &rc, DT_CENTER | DT_VCENTER);
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

ATOM TipMsgWindow::MyRegisterClass(HINSTANCE hInstance)
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
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"ShowTextTip";
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

bool TipMsgWindow::ShowATopMostInfoWindow()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance))
	{
		DOLOG("InitInstance Failed! ErrorCode:" + GetLastError());
		return false;
	}
	
	return true;
}

UINT TipMsgWindow::loopMsg()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}