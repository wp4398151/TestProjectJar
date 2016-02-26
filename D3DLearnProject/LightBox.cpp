#include "LightBox.h"
#include <assert.h>

// 顶点的结构
struct VertexType
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
};

VertexType g_LightBoxVertices[] = {
	//{ D3DXVECTOR3(-1.0f, -1.0f, 0.1f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f) },
	//{ D3DXVECTOR3(0.0f, 1.0f, 0.1f), D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f) },
	//{ D3DXVECTOR3(1.0f, -1.0f, 0.1f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f) }

	{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, -1.0f, -1.0f), },
	{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(-1.0f, 1.0f, -1.0f), },
	{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(1.0f, 1.0f, -1.0f), },

	{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, -1.0f, -1.0f), },
	{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(-1.0f, -1.0f, 1.0f), },
	{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 1.0f, 1.0f), },

	{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), },
	{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(1.0f, -1.0f, 1.0f), },
};

unsigned int g_LightBoxIndexVertice[] = {
	0,  // 前面 
	1,
	2,
	0,
	2,
	3,

	4,  // 后面 
	6,
	5,
	4,
	7,
	6,

	4,  // 左面 
	5,
	1,
	4,
	1,
	0,

	3,  //右面 
	2,
	6,
	3,
	6,
	7,

	1,  // 上面 
	5,
	6,
	1,
	6,
	2,

	4, // 下面 
	0,
	3,
	4,
	3,
	7
};

LightBox::LightBox(void)
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_vertexCount = sizeof(g_LightBoxVertices) / sizeof(g_LightBoxVertices[0]);
	m_indexCount = sizeof(g_LightBoxIndexVertice) / sizeof(g_LightBoxIndexVertice[0]);
}
LightBox::LightBox(const LightBox& rBox)
{

}

LightBox::~LightBox(void)
{

}

bool LightBox::Initialize(ID3D11Device* pDevice)
{
	//D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixTranslation(&m_WorldMatrix, 4.0, 0.0, 0.0);
	return InitializeBuffers(pDevice);
}

void LightBox::Shutdown()
{
	ShutdownBuffers();
}

void LightBox::Render(ID3D11DeviceContext* pDeviceContext)
{
	RenderBuffers(pDeviceContext);
}

int LightBox::GetIndexCount()
{
	return m_indexCount;
}


void LightBox::GetWorldMatrix(D3DXMATRIX& WorldMatrix)
{
	// 当前不用单独设置
	WorldMatrix = m_WorldMatrix;
}

bool LightBox::InitializeBuffers(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC vertexBufferDesc;	// 顶点缓存的描述
	D3D11_BUFFER_DESC indexBufferDesc; // 顶点索引缓存的描述
	D3D11_SUBRESOURCE_DATA vertexData; // 顶点需要访问的资源
	D3D11_SUBRESOURCE_DATA indexData; // 顶点索引需要访问的资源
	HRESULT result = true;

	// 设置顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 将资源绑定到顶点，供管线访问
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	vertexData.pSysMem = g_LightBoxVertices; // 将三角形的顶点数据放入顶点缓冲中
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(result));

	// 设置顶点索引缓冲描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(g_LightBoxIndexVertice[0]) *m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 将资源绑定到顶点索引，供管线访问
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	indexData.pSysMem = g_LightBoxIndexVertice; // 将三角形的顶点数据放入顶点缓冲中
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	assert(SUCCEEDED(result));
	return true;
}

void LightBox::ShutdownBuffers()
{
	// 释放顶点缓冲.
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}

	// 释放索引缓冲
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void LightBox::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
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
}