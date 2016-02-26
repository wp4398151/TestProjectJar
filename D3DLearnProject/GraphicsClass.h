#pragma once

#include <windows.h>
#include "d3dclass.h"
#include "CameraClass.h"
#include "ShaderClass.h"
#include "TriangleClass.h"
#include "Box.h"
#include "LightBox.h"
#include "LightClass.h"
#include "LightShaderClass.h"

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

		CameraClass *m_pCamera;
	private:
		bool Render();

		//����һ��D3DClass���Ա����
		D3DClass* m_D3D;
		ShaderClass *m_pShaderClass;
		LightShaderClass *m_pLightShaderClass;
		TriangleClass *m_pTriangleClass;
		Box *m_pBox;
		LightBox* m_pLightBox;
		LightClass* m_pLightClass;
	};

