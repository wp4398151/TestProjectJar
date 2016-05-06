////////////////////////////////////////////////////
// Show Tip On The Top Most Desktop.
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#pragma once

#include <Windows.h>

class TipMsgWindow
{
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool Init(LPSTR lpTextMsg, POINT& pos, POINT& size);
	BOOL InitInstance(HINSTANCE hInstance);
	ATOM MyRegisterClass(HINSTANCE hInstance);
	UINT loopMsg();
	bool ShowATopMostInfoWindow();
private:
	char m_Msg[MAX_PATH];
	POINT m_pos;
	POINT m_size;
};