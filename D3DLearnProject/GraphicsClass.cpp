#include "GraphicsClass.h"


GraphicsClass::GraphicsClass(void)
{
	m_D3D = NULL;
	m_pCamera = NULL;
	m_pTriangleClass = NULL;
	m_pShaderClass = NULL;
	m_pLightShaderClass = NULL;
	m_pBox = NULL;
	m_pLightBox = NULL;
	m_pLightClass = NULL;
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
	m_pCamera = new CameraClass;
	if (!m_pCamera)
	{
		return false;
	}
	// ���������λ��.
	m_pCamera->SetPosition(0.0f, 0.0f, -10.0f);

	// ��ʼ��������
	m_pTriangleClass = new TriangleClass;
	if (!m_pTriangleClass)
	{
		return false;
	}
	m_pTriangleClass->Initialize(m_D3D->GetDevice(), L"Test.dds");

	// ��ʼ������
	m_pBox = new Box;

	m_pBox->Initialize(m_D3D->GetDevice());
	if (!m_pBox)
	{
		return false;
	}
	
	// ��ʼ��Shader
	m_pShaderClass = new ShaderClass();
	if (!m_pShaderClass)
	{
		return false;
	}
	m_pShaderClass->Init(m_D3D->GetDevice(), hwnd);

	
	m_pLightBox = new LightBox;
	m_pLightBox->Initialize(m_D3D->GetDevice());
	if (!m_pLightBox)
	{
		return false;
	}

	m_pLightShaderClass = new LightShaderClass();
	if (!m_pShaderClass)
	{
		return false;
	}
	m_pLightShaderClass->Init(m_D3D->GetDevice(), hwnd);

	m_pLightClass = NULL;
	m_pLightClass = new LightClass;
	if (m_pLightClass)
	{
		return false;
	}
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

	if (m_pLightShaderClass)
	{
		m_pLightShaderClass->Shutdown();
		delete m_pLightShaderClass;
		m_pLightShaderClass = NULL;
	}

	if (m_pLightBox)
	{
		m_pLightBox->Shutdown();
		delete m_pLightBox;
		m_pLightBox = NULL;
	}
	
	//����m_D3D����
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
				3, worldMatrix, viewMatrix, projectionMatrix, m_pTriangleClass->GetTexture());
	
	D3DXVECTOR4 Ke = D3DXVECTOR4(0.8, 0.0, 0.2, 1.0);
	D3DXVECTOR4 Ka = D3DXVECTOR4(0.2, 0.2, 0.2, 1.0);
	D3DXVECTOR4 Kd = D3DXVECTOR4(1.0, 1.0, 1.0, 1.0);
	D3DXVECTOR4 Ks = D3DXVECTOR4(1.0, 1.0, 1.0, 1.0);

	m_pLightBox->Render(m_D3D->GetDeviceContext());
	D3DXVECTOR4	realcamerpos = D3DXVECTOR4(m_pCamera->m_PosVector.x, m_pCamera->m_PosVector.y, m_pCamera->m_PosVector.z, 1.0);
	result = m_pLightShaderClass->Render(m_D3D->GetDeviceContext(),
		m_pLightBox->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_pLightClass->GetPosition(), m_pLightClass->GetLightColor(), m_pLightClass->GetGlobalAmbient(),
		realcamerpos, Ke, Ka, Kd, Ks, m_pLightClass->GetDirection(), m_pLightClass->GetShininess());

	//��framebuffer�е�ͼ��present����Ļ��.
	m_D3D->EndScene();

	return true;
}