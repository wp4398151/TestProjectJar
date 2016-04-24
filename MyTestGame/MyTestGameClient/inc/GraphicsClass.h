#pragma once

#include <windows.h>
#include "d3dclass.h"
#include "CameraClass.h"
#include "RectClass.h"
#include "TextureManager.h"
#include "Role.h"

// 每帧的间隔时间
#define DEFAULTINTERVAL 30

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
	bool Frame(int elapse);

	CameraClass *m_pCamera;
	CRole	m_Role;
private:
	bool Render();

	//定义一个D3DClass类成员变量
	
	Vec2 m_mapSize;
	D3DClass* m_D3D;
	RectClass* m_pRect;
	TextureManager *m_pTextureManager;
};