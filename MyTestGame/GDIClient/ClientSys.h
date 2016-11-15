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
	wstring m_wstrTitle;							// �������ı�
	wstring m_wstrWindowClass;						// ����������
	LPWSTR m_lpwszTempMenu;							// ������
	POINT m_ptSize;									// ���������С
	HACCEL m_hAccelTable;							// ��ݼ�����
	int	m_nCmdShow;
	HICON m_hIcon;
	HICON m_hIconSm;

	HINSTANCE m_hInst;								// ��ǰʵ��
	HWND m_hWndMain;
};

