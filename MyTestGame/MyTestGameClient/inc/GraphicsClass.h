#pragma once

#include <windows.h>
#include "d3dclass.h"
#include "CameraClass.h"
#include "RectClass.h"
#include "TextureManager.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;//是否全屏
const bool VSYNC_ENABLED = true;//是否垂直同步
const float SCREEN_DEPTH = 1000.0f;//深度，远点
const float SCREEN_NEAR = 0.1f;//深度，近点

class GraphicsClass
{
public:
	GraphicsClass(void);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass(void);
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	CameraClass *m_pCamera;
private:
	bool Render();

	//定义一个D3DClass类成员变量
	D3DClass* m_D3D;
	RectClass* m_pRect;
	TextureManager *m_pTextureManager;
};