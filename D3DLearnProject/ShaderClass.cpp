#include "ShaderClass.h"
#include <D3Dcompiler.h>
#include <D3DX11async.h>
#include <assert.h>

struct MatrixBufferType
{
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
};

// 将Shader 直接写到字符串中,不用管理Shader文件
static unsigned char s_lpShaderContentStr[] =
"Texture2D shaderTexture;		"
"SamplerState SampleType;		"
"cbuffer MatrixBuffer{			"
"	matrix worldMatrix;			"
"	matrix viewMatrix;				"
"	matrix projectionMatrix;		"
"};								"
"struct VertexInputType{		"
"	float4 position : POSITION;	"		// 位置
"	float4 color : COLOR;		"		// 颜色
"	float2 texcoord : TEXCOORD0;"		// 贴图坐标
"};								"
"struct PixelInputType{			"
"	float4 position : SV_POSITION;			"
"	float4 color : COLOR;			"
"	float2 texcoord : TEXCOORD0;"
"};								"
"PixelInputType ColorVertexShader(VertexInputType input){		"
"	PixelInputType output;										"
"	input.position.w = 1.0f;									"
"	output.position = mul(input.position, worldMatrix);			"
"	output.position = mul(output.position, viewMatrix);			"
"	output.position = mul(output.position, projectionMatrix);	"
"	output.color = input.color;									"
"	output.texcoord = input.texcoord;							"
"	return output;												"
"}								"
"float4 ColorPixelShader(PixelInputType input) : SV_TARGET{		"
"	float4 textureColor;										"
"	textureColor = shaderTexture.Sample(SampleType, input.texcoord);"
"	return textureColor;										"
"}																"
"float4 ColorPixelShaderJustColor(PixelInputType input) : SV_TARGET{"
"	return input.color;											"
"}";

bool ShaderClass::Render(ID3D11DeviceContext *pDeviceContext,
			int indexCount,
			D3DXMATRIX worldMatrix,
			D3DXMATRIX viewMatrix,
			D3DXMATRIX projectionMatrix, 
			ID3D11ShaderResourceView* pShaderResourceView)
{
	//pDeviceContext->VSSetConstantBuffers(,ID3D11Buffer);
	bool res = true;
	res = SetShaderParam(pDeviceContext,
		worldMatrix, viewMatrix, projectionMatrix, pShaderResourceView);
	assert(res);
	RenderShader(pDeviceContext, indexCount);
	return res;
}


void ShaderClass::Shutdown()
{
	if (m_pSampleState)
	{
		m_pSampleState->Release();
	}
	if (m_pLayout)
	{
		m_pLayout->Release();
	}
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
	}
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
	}
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
	}
}

bool ShaderClass::Init(ID3D11Device *pDevice, HWND hwnd)
{
	HRESULT res = S_OK;
	ID3D10Blob *pErr = NULL; // 存放编译shader的错误信息
	ID3D10Blob *pVertexShaderBuffer = NULL; // 编译出来的VS字节
	ID3D10Blob *pPixelShaderBuffer = NULL; // 编译出来的PS字节
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3]; // 描述
	unsigned int numElem = sizeof(polygonLayout)/sizeof(polygonLayout[0]); // 一个点中的成员量
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };

	// 编译VS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpShaderContentStr,
		sizeof(s_lpShaderContentStr),
		NULL, NULL, NULL,
		"ColorVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));
	
	// 编译PS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpShaderContentStr,
		sizeof(s_lpShaderContentStr),
		NULL, NULL, NULL,
		"ColorPixelShader",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pPixelShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));
	// 创建VS Shader
	res = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
								pVertexShaderBuffer->GetBufferSize(),
								NULL, &m_pVertexShader);
	assert(SUCCEEDED(res));
		
	// 创建PS Shader
	res = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
								pPixelShaderBuffer->GetBufferSize(),
								NULL, &m_pPixelShader);
	assert(SUCCEEDED(res));

	// 创建定点布局，在Input-Assemble 阶段使用
	polygonLayout[0].SemanticName = "POSITION";	// 语义
	polygonLayout[0].SemanticIndex = 0;	// 语义
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// 语义
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";	// 语义
	polygonLayout[1].SemanticIndex = 0;	// 语义
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// 语义
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";	// 语义
	polygonLayout[2].SemanticIndex = 0;	// 语义
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;	// 语义
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;
	
	res = pDevice->CreateInputLayout(polygonLayout,
		numElem,
		pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(),
		&m_pLayout);
	assert(SUCCEEDED(res));

	pVertexShaderBuffer->Release();
	pVertexShaderBuffer = NULL;
	pPixelShaderBuffer->Release();
	pPixelShaderBuffer = NULL;
	
	// 创建Content 矩阵Buffer	
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	
	res = pDevice->CreateBuffer(&matrixBufferDesc, 
								NULL, 
								&m_pConstantBuffer);
	assert(SUCCEEDED(res));
	
	// 创建纹理采样描述
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// 线性插值方式采样
	// 地址模式
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// 创建纹理采样状态.
	res = pDevice->CreateSamplerState(&samplerDesc, &m_pSampleState);
	assert(SUCCEEDED(res));
	if (FAILED(res))
	{
		return false;
	}
	return true;
}

bool ShaderClass::RenderShader(ID3D11DeviceContext *pDeviceContext,
					int indexCount)
{
	pDeviceContext->IASetInputLayout(m_pLayout);
	// 设置VS Shader 和PS Shader
	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
	
	pDeviceContext->PSSetSamplers(0, 1, &m_pSampleState);

	pDeviceContext->DrawIndexed(indexCount, 0, 0);
	return true;
}

bool ShaderClass::SetShaderParam(ID3D11DeviceContext *pDeviceContext,
								D3DXMATRIX worldMatrix,
								D3DXMATRIX viewMatrix,
								D3DXMATRIX projectionMatrix,
								ID3D11ShaderResourceView* pShaderResourceView)
{
	HRESULT res = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *pBufferPtr = NULL;
	unsigned int bufferNum = NULL;
	
	// 传入之前需要转置	
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	
	res = pDeviceContext->Map(m_pConstantBuffer, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &mappedResource);
	assert(SUCCEEDED(res));

	pBufferPtr = (MatrixBufferType*)mappedResource.pData;
	pBufferPtr->world = worldMatrix;
	pBufferPtr->view = viewMatrix;
	pBufferPtr->projection= projectionMatrix;

	pDeviceContext->Unmap(m_pConstantBuffer, 0);

	bufferNum = 0;
	// 设置如VS Constant Buffer
	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &m_pConstantBuffer);
	
	// 设置ps shader资源.
	pDeviceContext->PSSetShaderResources(0, 1, &pShaderResourceView);

	return true;
}