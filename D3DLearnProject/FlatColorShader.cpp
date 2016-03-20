#include "FlatColorShader.h"
#include <D3Dcompiler.h>
#include <D3DX11async.h>
#include <assert.h>


static unsigned char s_lpFlatColorSahder[] = 
"struct VertexInputType{		"
"	float4 position : POSITION;	"		// λ��
"	float4 color : COLOR;		"		// ��ɫ
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
"float4 ColorPixelShader(PixelInputType input) : SV_TARGET{"
"	return input.color;											"
"}";

FlatColorShader::FlatColorShader()
{
	m_pLayout = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
}

FlatColorShader::~FlatColorShader()
{
}

void FlatColorShader::Shutdown()
{
	if (m_pLayout)
	{
		m_pLayout->Release();
		m_pLayout = NULL;
	}
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = NULL;
	}
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = NULL;
	}
}

bool FlatColorShader::Init(ID3D11Device* pDevice)
{
	HRESULT res = S_OK;
	ID3D10Blob *pErr = NULL; // ��ű���shader�Ĵ�����Ϣ
	ID3D10Blob *pVertexShaderBuffer = NULL; // ���������VS�ֽ�
	ID3D10Blob *pPixelShaderBuffer = NULL; // ���������PS�ֽ�
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2]; // ����
	unsigned int numElem = sizeof(polygonLayout)/sizeof(polygonLayout[0]); // һ�����еĳ�Ա��
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };

	// ����VS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpFlatColorSahder,
		sizeof(s_lpFlatColorSahder),
		NULL, NULL, NULL,
		"ColorVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));

	// ����PS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpFlatColorSahder,
		sizeof(s_lpFlatColorSahder),
		NULL, NULL, NULL,
		"ColorPixelShader",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pPixelShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));
	// ����VS Shader
	res = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(),
		NULL, &m_pVertexShader);
	assert(SUCCEEDED(res));
	// ����PS Shader
	res = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
		pPixelShaderBuffer->GetBufferSize(),
		NULL, &m_pPixelShader);
	assert(SUCCEEDED(res));

	// �������㲼�֣���Input-Assemble �׶�ʹ��
	polygonLayout[0].SemanticName = "POSITION";	// ����
	polygonLayout[0].SemanticIndex = 0;	// ����
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// ����
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";	// ����
	polygonLayout[1].SemanticIndex = 0;	// ����
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// ����
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
	RenderShader(pDeviceContext, indexCount);
	return true;
}

void FlatColorShader::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(m_pLayout);
	// ����VS Shader ��PS Shader
	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	pDeviceContext->DrawIndexed(indexCount, 0, 0);
	return ;		
}

const static FlatTriangle::VertexType s_vertices[] = {
		{ D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(0.5f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f) },
};
const static unsigned int s_indexVertice[sizeof(s_vertices) / sizeof(s_vertices[0])] = {
		0, 1, 2 };

FlatTriangle::FlatTriangle(void)
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_vertexCount = (sizeof(s_vertices) / sizeof(s_vertices[0]));
	m_indexCount = m_vertexCount;
}

bool FlatTriangle::Initialize(ID3D11Device* pDevice)
{
	bool res = false;
	res = InitializeBuffers(pDevice);
	assert(res);
	return res;
}

void FlatTriangle::Shutdown()
{
	ShutdownBuffers();
}

void FlatTriangle::Render(ID3D11DeviceContext* pDeviceContext)
{
	RenderBuffers(pDeviceContext);
}

int FlatTriangle::GetIndexCount()
{
	return m_indexCount;
}

bool FlatTriangle::InitializeBuffers(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC vertexBufferDesc;	// ���㻺�������
	D3D11_BUFFER_DESC indexBufferDesc; // �����������������
	D3D11_SUBRESOURCE_DATA vertexData; // ������Ҫ���ʵ���Դ
	D3D11_SUBRESOURCE_DATA indexData; // ����������Ҫ���ʵ���Դ
	HRESULT result = true;

	// ���ö��㻺������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* (sizeof(s_vertices) / sizeof(s_vertices[0]));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ����Դ�󶨵����㣬�����߷���
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����.
	vertexData.pSysMem = s_vertices; // �������εĶ������ݷ��붥�㻺����
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������㻺��.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(result));

	// ���ö���������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(s_indexVertice[0]) * (sizeof(s_vertices) / sizeof(s_vertices[0]));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // ����Դ�󶨵����������������߷���
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����.
	indexData.pSysMem = s_indexVertice; // �������εĶ������ݷ��붥�㻺����
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �������㻺��.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	assert(SUCCEEDED(result));

	return true;
}

void FlatTriangle::ShutdownBuffers()
{
	// �ͷŶ��㻺��.
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}

	// �ͷ���������
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void FlatTriangle::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int stride = 0;
	unsigned int offset = 0;

	// ���ö��㻺���Ⱥ�ƫ��.
	stride = sizeof(VertexType);
	offset = 0;

	//��input assemberl�׶ΰ󶨶��㻺�壬�Ա��ܹ�����Ⱦ
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//��input assemberl�׶ΰ��������壬�Ա��ܹ�����Ⱦ
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ������Ԫ���壬��Ⱦ�������б�.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
