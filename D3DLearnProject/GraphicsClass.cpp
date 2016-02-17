#include "GraphicsClass.h"


GraphicsClass::GraphicsClass(void)
{
	m_D3D = 0;
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

	// ����һ��D3DClass����.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// ����D3DClass��ʼ������
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	
	// ��ʼ�������
	// �������������
	m_pCarema = new CameraClass;
	if (!m_pCarema)
	{
		return false;
	}
	// ���������λ��.
	m_pCarema->SetPosition(0.0f, 0.0f, -10.0f);

	// ��ʼ��������
	m_pTriangleClass = new TriangleClass;
	if (!m_pTriangleClass)
	{
		return false;
	}
	m_pTriangleClass->Initialize(m_D3D->GetDevice());
	
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
	}
	if (m_pTriangleClass)
	{
		m_pTriangleClass->Shutdown();
	}
	//����m_D3D����
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	// ����Render��������Ⱦ3D����
	// Render��GraphicsClass��˽�к���.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix;	// ����任 
	D3DXMATRIX viewMatrix;	// �ӵ�任����
	D3DXMATRIX projectionMatrix;	// ͶӰ�任��֤
	
	bool result = true;;
	// ����framebuffer.Ϊǳ��ɫ
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);

	//��framebuffer�е�ͼ��present����Ļ��.
	m_D3D->EndScene();

	return true;
}