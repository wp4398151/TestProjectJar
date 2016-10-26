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
	HINSTANCE m_hInst;								// ��ǰʵ��
	wstring m_wstrTitle;						// �������ı�
	wstring m_wstrWindowClass;				// ����������
	POINT m_ptSize;									// ���������С
	HACCEL m_hAccelTable;							// ��ݼ�����
	int	m_nCmdShow;
	HWND m_hWndMain;
};

