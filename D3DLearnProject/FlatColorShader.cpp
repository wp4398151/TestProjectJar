#include "FlatColorShader.h"
#include <D3Dcompiler.h>
#include <D3DX11async.h>


static unsigned char s_lpFlatColorSahder[] = 
"struct VertexInputType{		"
"	float4 position : POSITION;	"		// 位置
"	float4 color : COLOR;		"		// 颜色
"};								"
"struct PixelInputType{			"
"	float4 position : SV_POSITION;			"
"	float4 color : COLOR;			"
"};								"
"PixelInputType ColorVertexShader(VertexInputType input){		"
"	PixelInputType output;										"
"	input.position.w = 1.0f;									"
"	output.color = input.color;									"
"	output.position = input.position;							"
"	return output;												"
"}								"
"float4 ColorPixelShaderJustColor(PixelInputType input) : SV_TARGET{"
"	return input.color;											"
"}";

FlatColorShader::FlatColorShader()
{

}

FlatColorShader::~FlatColorShader()
{

}

void FlatColorShader::Shutdown()
{

}

bool FlatColorShader::Init(ID3D11Device* pDevice)
{
	HRESULT res = S_OK;
	ID3D10Blob *pErr = NULL; // 存放编译shader的错误信息
	ID3D10Blob *pVertexShaderBuffer = NULL; // 编译出来的VS字节
	ID3D10Blob *pPixelShaderBuffer = NULL; // 编译出来的PS字节
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2]; // 描述
	unsigned int numElem = sizeof(polygonLayout)/sizeof(polygonLayout[0]); // 一个点中的成员量
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };

	// 编译VS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpFlatColorSahder,
		sizeof(s_lpFlatColorSahder),
		NULL, NULL, NULL,
		"ColorVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));

	// 编译PS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpFlatColorSahder,
		sizeof(s_lpFlatColorSahder),
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

	return true;
}


bool FlatColorShader::Render(ID3D11DeviceContext *pDeviceContext, int indexCount)
{
	RenderShader(pDeviceContext);
	return true;
}

void FlatColorShader::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount)
{

	return ;		
}