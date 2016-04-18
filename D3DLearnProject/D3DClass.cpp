#include "D3DClass.h"
#include <assert.h>

D3DClass::D3DClass(void)
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_pBlendEnableState = 0;
	m_pBlendDisableState = 0;
	m_pDepthStencilStateMirror = 0;
	m_pDepthStencilStateReflect = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{

}

D3DClass::~D3DClass(void)
{
}

//Initialize函数包含完成D3D设置的所有代码。
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
	float screenDepth, float screenNear)
{
	HRESULT result = NULL;
	IDXGIFactory* factory = NULL;
	IDXGIAdapter* adapter = NULL;
	IDXGIOutput* adapterOutput = NULL;
	unsigned int numModes = 0;
	unsigned int n = 0;
	unsigned int i = 0;
	unsigned int numerator = 0;
	unsigned int denominator = 1;
	unsigned int stringLength = 0;
	DXGI_MODE_DESC* displayModeList = NULL;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr = NULL;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	if (screenWidth < 1)
		screenWidth = 1;
	if (screenHeight <1)
		screenHeight = 1;

	// 保存垂直同步设置
	m_vsync_enabled = vsync;

	// 创建一个DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// 用接口工厂创建一个主显卡的适配
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return false;
	}

	// 得到主适配器的输出.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	//得到适合 DXGI_FORMAT_R8G8B8A8_UNORM 的显示模式.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// 保存显示模式到displayModeList中
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	//遍历所有显示模式，得到刷新率两个参数值numerator 和 denominato
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}
	// 得到显卡描述
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// 保存显存大小.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//保存显卡描述串
	//wcstombs_s, wide char转化为char
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}
	// 释放显示模式列表
	delete [] displayModeList;
	displayModeList = 0;

	//释放适配器输出.
	adapterOutput->Release();
	adapterOutput = 0;

	//释放适配器
	adapter->Release();
	adapter = 0;

	// 释放接口工厂.
	factory->Release();
	factory = 0;

	// 初始化交换链描述
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 用1个后缓冲
	swapChainDesc.BufferCount = 1;

	//帧缓冲的大小和应用程序窗口大小相等.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 后缓冲的surface的格式为DXGI_FORMAT_R8G8B8A8_UNORM.
	// surface的每个像素用4个无符号的8bit[映射到0-1]来表示。NORM表示归一化。
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 如果使用垂直同步，设置后缓冲的刷新率。.
	//刷新率就是一秒钟把后缓冲内容在屏幕上画出的次数。
	//如果开启垂直同步，则锁定刷新率，则fps是固定的
	if(m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 设置后缓冲的用途
	// 我们的渲染目标缓冲为后缓冲。
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 后缓冲输出的窗口句柄.
	swapChainDesc.OutputWindow = hwnd;

	// 不使用多重采样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 设置全屏或者窗口模式.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 设定扫描线ordering以及缩放为unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 后缓冲内容呈现到屏幕后，放弃其内容
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//不设置标志
	swapChainDesc.Flags = 0;

	// 设置feature level为D3D11
	// 如果显卡不支持D3D11,我们能够通过设置这个参数，使用D3D10,或者9.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 创建交换链，设备以及设备上下文.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if(FAILED(result))
	{
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
			D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
		
		if (FAILED(result))
		{
			return false;
		}
	}

	// 得到交换链中的后缓冲指针.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// 用后缓冲创建渲染目标视图.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	//释放后缓冲.(引用计数减1)
	backBufferPtr->Release();
	backBufferPtr = 0;

	// 初始化深度缓冲描述.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//设置深度缓冲描述
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;//对于深度缓冲为1
	depthBufferDesc.ArraySize = 1;//对于深度缓冲为1，对于纹理，这2个参数有更多用途
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 创建深度缓冲.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// 初始化深度模版状态描述.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 设置深度模版状态描述.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//D3D11_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 对于front face 像素使用的模版操作操作.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 对于back face像素使用的模版操作模式.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 创建深度模版状态，使其生效
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// 设置深度模版状态.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	// 创建镜像深度模板
	D3D11_DEPTH_STENCIL_DESC mirrorStateDesc;
	ZeroMemory(&mirrorStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	mirrorStateDesc.DepthEnable = true;
	mirrorStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	mirrorStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

	mirrorStateDesc.StencilEnable = true;
	mirrorStateDesc.StencilReadMask = 0xFF;
	mirrorStateDesc.StencilWriteMask = 0xFF;
	// front face 的模板操作
	mirrorStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// back fase 的模板操作
	mirrorStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	mirrorStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result= m_device->CreateDepthStencilState(&mirrorStateDesc, &m_pDepthStencilStateMirror);
	assert(SUCCEEDED(result));

	mirrorStateDesc.DepthEnable = true;
	mirrorStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;//D3D11_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲 
	mirrorStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	mirrorStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mirrorStateDesc.FrontFace.StencilFunc= D3D11_COMPARISON_EQUAL;
	result = m_device->CreateDepthStencilState(&mirrorStateDesc, &m_pDepthStencilStateReflect);

	assert(SUCCEEDED(result));

	// 初始化深度模版视图.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 设置深度模版视图描述.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 创建深度模版视图.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}
	// 绑定渲染目标视图和深度缓冲到渲染管线.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 设置光栅化描述，指定多边形如何被渲染.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;	// 实体填充
	//rasterDesc.FillMode = D3D11_FILL_WIREFRAME;	// 填充线段
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 创建光栅化状态
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		return false;
	}

	//设置光栅化状态，使其生效
	m_deviceContext->RSSetState(m_rasterState);

	// create blend state 
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory((void*)&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;//0x0f;
	result = m_device->CreateBlendState(&blendDesc, &m_pBlendEnableState);
	assert(SUCCEEDED(result));

	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	result = m_device->CreateBlendState(&blendDesc, &m_pBlendDisableState);
	assert(SUCCEEDED(result));

	// 设置视口，显示全部后缓冲内容
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;

	viewport.MinDepth = 0.0f;	// 仅仅表示深度缓冲区的值,而不是是椎体的
	viewport.MaxDepth = 1.0f;

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// 创建视口,用来指定显示到屏幕的哪一块区域，以及深度剔除
	m_deviceContext->RSSetViewports(1, &viewport);

	// 设置透视投影矩阵
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 创建透视投影矩阵.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//初始化world矩阵为单位矩阵.
	//该矩阵实现局部坐标到世界坐标的转换
	D3DXMatrixIdentity(&m_worldMatrix);

	// 创建正交投影矩阵，主要用来实施2D渲染.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	// 释放交换链资源前，先设置为窗口模式，否则可能会产生异常.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// 设置清除后缓冲颜色.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//清除后缓冲.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//清除深度缓冲.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	//渲染完成后，把后缓冲内容present到前缓冲
	if(m_vsync_enabled)
	{
		// 锁定屏幕刷新率.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 尽可能快的present后缓冲.
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

IDXGISwapChain* D3DClass::GetSwapChain()
{
	return m_swapChain;
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_deviceContext->OMSetBlendState(m_pBlendEnableState, blendFactor, 0xffffffff);
}

void D3DClass::ChangeBackCullMode(bool bIsCull)
{
	HRESULT hr = S_OK;
	if (m_rasterState)
	{
		m_rasterState->Release();
	}
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	//if (bIsCull)
	//{
	//	rasterDesc.CullMode = D3D11_CULL_BACK;
	//}
	//else
	//{
	//	rasterDesc.CullMode = D3D11_CULL_NONE;
	//}
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_SOLID

	rasterDesc.FrontCounterClockwise = bIsCull;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	assert(SUCCEEDED(hr));
	m_deviceContext->RSSetState(m_rasterState);
}

void D3DClass::EnableMirrorDepthStancil()
{
	m_deviceContext->OMSetDepthStencilState(m_pDepthStencilStateMirror, 1);
}

void D3DClass::EnableReflectDepthStancil()
{
	m_deviceContext->OMSetDepthStencilState(m_pDepthStencilStateReflect, 1);
}

void D3DClass::EnableDefaultDepthStencil()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
}

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_deviceContext->OMSetBlendState(m_pBlendDisableState, blendFactor, 0xffffffff);
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

