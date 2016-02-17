#pragma once

#include <windows.h>
#include "d3dclass.h"
#include "CameraClass.h"
#include "ShaderClass.h"
#include "TriangleClass.h"

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
		bool Frame();

	private:
		bool Render();

		//����һ��D3DClass���Ա����
		D3DClass* m_D3D;
		CameraClass *m_pCarema;
		ShaderClass *m_pShaderClass;
		TriangleClass *m_pTriangleClass;
	};

