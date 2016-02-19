#include "TriangleClass.h"
#include <assert.h>
// 顶点的结构
struct VertexType
{
	D3DXVECTOR3 position;
	D3DXVECTOR4 color;
	D3DXVECTOR2 texcoord;
};
VertexType g_vertices[] = {
	//{ D3DXVECTOR3(-1.0f, -1.0f, 0.1f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f) },
	//{ D3DXVECTOR3(0.0f, 1.0f, 0.1f), D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f) },
	//{ D3DXVECTOR3(1.0f, -1.0f, 0.1f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f) }

	//{ D3DXVECTOR3(-0.5f, -.5f, 10.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f) },
	//{ D3DXVECTOR3(0.0f, 0.5f, 10.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f) },
	//{ D3DXVECTOR3(0.5f, -0.5f, 10.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f) }
	{ D3DXVECTOR3(0.0f, -1.0f, 1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f,0.0f)},
	{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f,0.0f)},
	{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f,0.1f)},
};
unsigned int g_indexVertice[sizeof(g_vertices) / sizeof(g_vertices[0])] = {
	0, 1, 2 };

TriangleClass::TriangleClass(void)
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_vertexCount = (sizeof(g_vertices) / sizeof(g_vertices[0]));
	m_indexCount = m_vertexCount;
}

TriangleClass::TriangleClass(const TriangleClass &)
{

}

TriangleClass::~TriangleClass(void)
{

}
bool TriangleClass::Initialize(ID3D11Device* pDevice)
{
	return InitializeBuffers(pDevice);
}

void TriangleClass::Shutdown()
{
	ShutdownBuffers();
}
void TriangleClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	RenderBuffers(pDeviceContext);
}

int TriangleClass::GetIndexCount()
{
	return m_indexCount;
}

bool TriangleClass::InitializeBuffers(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC vertexBufferDesc;	// 顶点缓存的描述
	D3D11_BUFFER_DESC indexBufferDesc; // 顶点索引缓存的描述
	D3D11_SUBRESOURCE_DATA vertexData; // 顶点需要访问的资源
	D3D11_SUBRESOURCE_DATA indexData; // 顶点索引需要访问的资源
	HRESULT result = true;
	
	// 设置顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (sizeof(g_vertices) / sizeof(g_vertices[0]));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 将资源绑定到顶点，供管线访问
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	vertexData.pSysMem = g_vertices; // 将三角形的顶点数据放入顶点缓冲中
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(result));
	
	// 设置顶点索引缓冲描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(g_indexVertice[0]) * (sizeof(g_vertices) / sizeof(g_vertices[0]));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 将资源绑定到顶点索引，供管线访问
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	indexData.pSysMem = g_indexVertice; // 将三角形的顶点数据放入顶点缓冲中
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	assert(SUCCEEDED(result));

	return true;
}


void TriangleClass::ShutdownBuffers()
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

void TriangleClass::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
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