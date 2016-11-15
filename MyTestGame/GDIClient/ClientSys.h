#pragma once
#include <windows.h>
#include <string>

using namespace std;

class ClientSys
{
public:
	struct ClientSysInitParam{
		HINSTANCE hInstance;
		int nCmdShow;
		wstring wstrAppTitle;
		wstring wstrWindowsClass;
		POINT ptSize;
		HACCEL hAccelTable;
		HICON hIcon;
		HICON hIconSm;
	};

public:
	ClientSys(ClientSysInitParam& rClientSysParam, LPWSTR lpwszTempMenu);
	virtual ~ClientSys();
	bool Initialize();
	int LoopMsg();

protected:
	bool InitInstance();
	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	ATOM MyRegisterClass();

private:
	wstring m_wstrTitle;							// 标题栏文本
	wstring m_wstrWindowClass;						// 主窗口类名
	LPWSTR m_lpwszTempMenu;							// 窗口名
	POINT m_ptSize;									// 绘制区域大小
	HACCEL m_hAccelTable;							// 快捷键保存
	int	m_nCmdShow;
	HICON m_hIcon;
	HICON m_hIconSm;

	HINSTANCE m_hInst;								// 当前实例
	HWND m_hWndMain;
};

