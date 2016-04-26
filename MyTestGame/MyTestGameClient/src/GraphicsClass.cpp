#include "GraphicsClass.h"
#include <assert.h>
#include "Helper.h"

GraphicsClass::GraphicsClass(void) :m_Role(DEFAULTINTERVAL), m_mapSize(0,0)
{
	m_D3D = NULL;
	m_pCamera = NULL;
	m_pRect = NULL;
	m_pTextureManager = NULL;
}

GraphicsClass::GraphicsClass(const GraphicsClass&):m_Role(DEFAULTINTERVAL)
{

}
GraphicsClass::~GraphicsClass(void)
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	Shutdown();
	m_mapSize.m_x = screenWidth;
	m_mapSize.m_y = screenHeight;

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
	m_pCamera->SetPosition(0.0f, 0.0f, -1.0f);

	m_pRect = new RectClass;
	assert(m_pRect);
	if (!m_pRect)
	{
		return false;
	}
	m_pRect->Initialize(m_D3D->GetDevice(), 30, 1.0f);
	
	m_pTextureManager = new TextureManager();
	assert(m_pTextureManager);
	if (!m_pTextureManager)
	{
		return false;
	}
	m_pTextureManager->Initialize(m_D3D->GetDevice());
	
	m_pTextureManager->LoadTextureFromFileW(m_D3D->GetDevice(), L"SingleTile.jpg");
	return true;
}

void GraphicsClass::Shutdown()
{
	//销毁m_D3D对象
	SAFE_DELETEEX(m_D3D, m_D3D->Shutdown());
	SAFE_DELETE(m_pCamera);
	SAFE_DELETEEX(m_pRect, m_pRect->Shutdown());
	return;
}

bool GraphicsClass::Frame(int elapse)
{
	bool result;
	// 调用Render函数，渲染3D场景
	// Render是GraphicsClass的私有函数.
	m_Role.Frame(elapse);
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix;			// 世界变换 
	D3DXMATRIX viewMatrix;			// 视点变换矩阵
	D3DXMATRIX projectionMatrix;	// 投影变换举证
	
	bool result = true;;
	// 设置framebuffer.为浅蓝色
	m_D3D->BeginScene(0.1f, 0.0f, 0.5f, 1.0f);

	m_pCamera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(projectionMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);

	// 构造一个旋转角度，让指定的物体，固定朝摄像机
	D3DXVECTOR3 posCamera = m_pCamera->GetPosition();

	// TODO: need to enumerate all rect
	//D3DXMATRIX rectEndMatrix;
	//D3DXMatrixIdentity(&rectEndMatrix);

	D3DXMATRIX rectEndMatrix;
	LPWSTR lpwTextureFile = NULL;

	//D3DXMatrixTranslation(&rectEndMatrix, 100.0, 0.0, 0.0);
	//lpwTextureFile = L"SingleTile.jpg";
	//m_pRect->Render(m_D3D->GetDeviceContext(),
	//	rectEndMatrix,
	//	viewMatrix,
	//	projectionMatrix,
	//	m_pTextureManager->LoadTextureFromFileW(m_D3D->GetDevice(), lpwTextureFile),
	//	m_pTextureManager->GetSamplaState());
	
	Vec2 pos;
	m_Role.getPos(pos);
	// 以左上方为(0,0)来变换成D3D中的坐标
	pos = pos - (m_mapSize / 2.0);
	pos.m_y *= -1;

	D3DXMatrixTranslation(&rectEndMatrix, pos.m_x, pos.m_y, 0.0);
	lpwTextureFile = m_Role.GetTextureFile();

	m_pRect->Render(m_D3D->GetDeviceContext(),
		rectEndMatrix,
		viewMatrix,
		projectionMatrix,
		m_pTextureManager->LoadTextureFromFileW(m_D3D->GetDevice(), lpwTextureFile),
		m_pTextureManager->GetSamplaState());

	m_D3D->EndScene();
	return true;
}