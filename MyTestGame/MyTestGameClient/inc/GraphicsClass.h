#pragma once

#include <windows.h>
#include "d3dclass.h"
#include "CameraClass.h"
#include "RectClass.h"
#include "TextureManager.h"
#include "Role.h"

// ÿ֡�ļ��ʱ��
#define DEFAULTINTERVAL 30

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;//�Ƿ�ȫ��
const bool VSYNC_ENABLED = true;//�Ƿ�ֱͬ��
const float SCREEN_DEPTH = 1000.0f;//��ȣ�Զ��
const float SCREEN_NEAR = 0.1f;//��ȣ�����

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

	//����һ��D3DClass���Ա����
	
	Vec2 m_mapSize;
	D3DClass* m_D3D;
	RectClass* m_pRect;
	TextureManager *m_pTextureManager;
};