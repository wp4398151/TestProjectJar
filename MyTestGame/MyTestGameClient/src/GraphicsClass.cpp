#include "GraphicsClass.h"
#include <assert.h>
#include "Helper.h"

GraphicsClass::GraphicsClass(void)
{
	m_D3D = NULL;
	m_pCamera = NULL;
	m_pRect = NULL;
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

	m_pRect = new RectClass;
	assert(m_pRect );
	if (!m_pRect)
	{
		return false;
	}
	m_pRect->Initialize(m_D3D->GetDevice());
	return true;
}

void GraphicsClass::Shutdown()
{
	//����m_D3D����
	SAFE_DELETEEX(m_D3D, m_D3D->Shutdown());
	SAFE_DELETE(m_pCamera);
	SAFE_DELETEEX(m_pRect, m_pRect->Shutdown());
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
	D3DXMATRIX worldMatrix;			// ����任 
	D3DXMATRIX viewMatrix;			// �ӵ�任����
	D3DXMATRIX projectionMatrix;	// ͶӰ�任��֤
	
	bool result = true;;
	// ����framebuffer.Ϊǳ��ɫ
	m_D3D->BeginScene(0.1f, 0.0f, 0.5f, 1.0f);

	m_pCamera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(projectionMatrix);

	// ����һ����ת�Ƕȣ���ָ�������壬�̶��������
	D3DXVECTOR3 posCamera = m_pCamera->GetPosition();

	// TODO: need to enumerate all rect
	D3DXMATRIX rectEndMatrix;
	m_pRect->Render(m_D3D->GetDeviceContext(), 
						rectEndMatrix, 
							viewMatrix, 
								projectionMatrix);

	m_D3D->EndScene();
	return true;
}