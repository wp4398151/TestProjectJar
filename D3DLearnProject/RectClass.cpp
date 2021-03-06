#include "RectClass.h"
#include <assert.h>
#include <D3DX11async.h>

struct MatrixBufferType
{
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
};

static RectClass::VertexType s_RectClassVertexs[] = {
	{ D3DXVECTOR3(-10.0f, 10.0f, 0.0f), D3DXVECTOR4(0.5f, 0.0f, 0.0f, 0.5f), D3DXVECTOR3(0.0f, 0.0f, -1.0f)},
	{ D3DXVECTOR3(10.0f, 10.0f, 0.0f), D3DXVECTOR4(0.5f, 0.0f, 0.0f, 0.5f), D3DXVECTOR3(0.0f, 0.0f, -1.0f)},
	{ D3DXVECTOR3(-10.0f, -10.0f, 0.0f), D3DXVECTOR4(0.5f, 0.0f, 0.0f, 0.5f), D3DXVECTOR3(0.0f, 0.0f, -1.0f)},
	{ D3DXVECTOR3(10.0f, -10.0f, 0.0f), D3DXVECTOR4(0.5f, 0.0f, 0.0f, 0.5f), D3DXVECTOR3(0.0f, 0.0f, -1.0f)},
};

static int s_RectClassIndexs[] = {0,1,2,2,1,3};

static char s_RectShaderStr[] =
"cbuffer MatrixBuffer{			"
"	matrix worldMatrix;			"
"	matrix viewMatrix;			"
"	matrix projectionMatrix;	"
"};								"
"struct VertexInputType{		"
"	float4 position : POSITION;	"		// 位置
"	float4 color : COLOR;		"		// 颜色
"	float4 normal : NORMAL;		"		// 法向 
"};								"
"struct PixelInputType{			"
"	float4 position : SV_POSITION;	"
"	float4 color : COLOR;			"
"};								"
"PixelInputType ColorVertexShader(VertexInputType input){		"
"	PixelInputType output;										"
"	input.position.w = 1.0f;									"
"	output.position = mul(input.position, worldMatrix);			"
"	output.position = mul(output.position, viewMatrix);			"
"	output.position = mul(output.position, projectionMatrix);	"
"	output.color = input.color;									"
"	return output;												"
"}								"
"float4 ColorPixelShaderColor(PixelInputType input) : SV_TARGET{"
"	return input.color;											"
"}";

RectClass::RectClass()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_IndexCount = sizeof(s_RectClassIndexs)/sizeof(s_RectClassIndexs[0]);
	m_vertexCount = sizeof(s_RectClassVertexs)/sizeof(s_RectClassVertexs[0]);

	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pLayout = NULL;
	m_pContantBuffer = NULL;
}

RectClass::~RectClass()
{

}

bool RectClass::Initialize(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC vertexBufferDesc;	// 顶点缓存的描述
	D3D11_BUFFER_DESC indexBufferDesc; // 顶点索引缓存的描述
	D3D11_SUBRESOURCE_DATA vertexData; // 顶点需要访问的资源
	D3D11_SUBRESOURCE_DATA indexData; // 顶点索引需要访问的资源
	HRESULT res = S_OK;

	// 设置顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(s_RectClassVertexs);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 将资源绑定到顶点，供管线访问
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	vertexData.pSysMem = s_RectClassVertexs; // 将三角形的顶点数据放入顶点缓冲中
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	res = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(res));

	// 设置顶点索引缓冲描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(s_RectClassIndexs);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 将资源绑定到顶点索引，供管线访问
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	indexData.pSysMem = s_RectClassIndexs; // 将三角形的顶点数据放入顶点缓冲中
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	res = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	assert(SUCCEEDED(res));

	// 初始化Shader
	ID3D10Blob *pErr = NULL; // 存放编译shader的错误信息
	ID3D10Blob *pVertexShaderBuffer = NULL; // 编译出来的VS字节
	ID3D10Blob *pPixelShaderBuffer = NULL; // 编译出来的PS字节
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3]; // 描述
	unsigned int numElem = sizeof(polygonLayout) / sizeof(polygonLayout[0]); // 一个点中的成员量
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };

	// 编译VS
	res = D3DX11CompileFromMemory((LPCSTR)s_RectShaderStr,
		sizeof(s_RectShaderStr),
		NULL, NULL, NULL,
		"ColorVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));

	// 编译PS
	res = D3DX11CompileFromMemory((LPCSTR)s_RectShaderStr,
		sizeof(s_RectShaderStr),
		NULL, NULL, NULL,
		"ColorPixelShaderColor",
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

	polygonLayout[2].SemanticName = "NORMAL";	// 语义
	polygonLayout[2].SemanticIndex = 0;	// 语义
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// 语义
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
		&m_pContantBuffer);
	assert(SUCCEEDED(res));

	return true;
}

void RectClass::ShutDown()
{
	if (m_pVertexBuffer) { m_pVertexBuffer->Release(); m_pVertexBuffer = NULL; }
	if (m_pIndexBuffer){ m_pIndexBuffer->Release(); m_pIndexBuffer = NULL; }

	if (m_pVertexShader) { m_pVertexShader->Release(); m_pVertexShader = NULL; }
	if (m_pPixelShader) { m_pPixelShader->Release(); m_pPixelShader = NULL; }
	if (m_pLayout) { m_pLayout->Release(); m_pLayout = NULL; }
	if (m_pContantBuffer) { m_pContantBuffer->Release(); m_pContantBuffer = NULL; }
}

void RectClass::Render(ID3D11DeviceContext* pDeviceContext, 
				D3DXMATRIX worldMatrix,
					D3DXMATRIX viewMatrix,
						D3DXMATRIX projectionMatrix)

{
	unsigned int stride = 0;
	unsigned int offset = 0;
	// 设置顶点缓冲跨度和偏移.
	stride = sizeof(VertexType);
	offset = 0;

	//在input assemberl阶段绑定顶点缓冲，以便能够被渲染
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//在input assemberl阶段绑定索引缓冲，以便能够被渲染
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 设置体元语义，渲染三角形列表.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 渲染Shader
	HRESULT res = S_OK;
	// 传入之前需要转置	
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	res = pDeviceContext->Map(m_pContantBuffer, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &mappedResource);
	assert(SUCCEEDED(res));

	
	MatrixBufferType* pBufferPtr = (MatrixBufferType*)mappedResource.pData;
	pBufferPtr->world = worldMatrix;
	pBufferPtr->view = viewMatrix;
	pBufferPtr->projection = projectionMatrix;

	pDeviceContext->Unmap(m_pContantBuffer, 0);

	// 设置如VS Constant Buffer
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pContantBuffer);

	pDeviceContext->IASetInputLayout(m_pLayout);
	// 设置VS Shader 和PS Shader
	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}