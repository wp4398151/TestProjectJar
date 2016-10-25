#pragma once
#include "Resource.h"
#include <windows.h>
#include <string>
using namespace std;

class ClientSys
{
public:
	ClientSys(HINSTANCE hInstance, int nCmdShow);
	virtual ~ClientSys();
	
	bool Initialize();
	int LoopMsg();

protected:
	bool InitInstance();
	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	ATOM MyRegisterClass();
private:
	HINSTANCE m_hInst;								// 当前实例
	wstring m_wstrTitle;						// 标题栏文本
	wstring m_wstrWindowClass;				// 主窗口类名
	POINT m_ptSize;									// 绘制区域大小
	HACCEL m_hAccelTable;							// 快捷键保存
	int	m_nCmdShow;
	HWND m_hWndMain;
};

