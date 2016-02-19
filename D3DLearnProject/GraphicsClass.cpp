#include "GraphicsClass.h"


GraphicsClass::GraphicsClass(void)
{
	m_D3D = NULL;
	m_pCamera = NULL;
	m_pTriangleClass = NULL;
	m_pShaderClass = NULL;
	m_pBox = NULL;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{

}
GraphicsClass::~GraphicsClass(void)
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	Shutdown();

	// 创建一个D3DClass对象.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// 调用D3DClass初始化函数
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	
	// 初始化摄像机
	// 创建摄像机对象
	m_pCamera = new CameraClass;
	if (!m_pCamera)
	{
		return false;
	}
	// 设置摄像机位置.
	m_pCamera->SetPosition(0.0f, 0.0f, -10.0f);

	// 初始化三角形
	m_pTriangleClass = new TriangleClass;
	if (!m_pTriangleClass)
	{
		return false;
	}
	m_pTriangleClass->Initialize(m_D3D->GetDevice());
	

	// 初始化矩形
	m_pBox = new Box;

	m_pBox->Initialize(m_D3D->GetDevice());
	if (!m_pBox)
	{
		return false;
	}
	
	// 初始化Shader
	m_pShaderClass = new ShaderClass();
	if (!m_pShaderClass)
	{
		return false;
	}
	m_pShaderClass->Init(m_D3D->GetDevice(), hwnd);
	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_pShaderClass)
	{
		m_pShaderClass->Shutdown();
		delete m_pShaderClass;
		m_pShaderClass = NULL;
	}
	if (m_pTriangleClass)
	{
		m_pTriangleClass->Shutdown();
		delete m_pTriangleClass;
		m_pTriangleClass = NULL;
	}

	if (m_pBox)
	{
		m_pBox->Shutdown();
		delete m_pBox;
		m_pBox = NULL;
	}
	
	//销毁m_D3D对象
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = NULL;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	// 调用Render函数，渲染3D场景
	// Render是GraphicsClass的私有函数.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix;	// 世界变换 
	D3DXMATRIX viewMatrix;	// 视点变换矩阵
	D3DXMATRIX projectionMatrix;	// 投影变换举证
	
	bool result = true;;
	// 设置framebuffer.为浅蓝色
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);
	
	m_pCamera->GetViewMatrix(viewMatrix);
	//m_pCamera->pitch(1.0f);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	//m_D3D->GetOrthoMatrix(projectionMatrix);

	//m_pBox->Render(m_D3D->GetDeviceContext());
	//result = m_pShaderClass->Render(m_D3D->GetDeviceContext(), 
	//			m_pBox->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	m_pTriangleClass->Render(m_D3D->GetDeviceContext());
	result = m_pShaderClass->Render(m_D3D->GetDeviceContext(), 
				3, worldMatrix, viewMatrix, projectionMatrix);
	//把framebuffer中的图像present到屏幕上.
	m_D3D->EndScene();

	return true;
}