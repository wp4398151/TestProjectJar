#include "AxisModelClass.h"


AxisModelClass::AxisModelClass(void)
{
}

AxisModelClass::AxisModelClass(const AxisModelClass & others)
{

}

AxisModelClass::~AxisModelClass(void)
{
}

bool AxisModelClass::Initialize(ID3D11Device* device)
{
	bool result = false;
	D3DXMatrixIdentity(&m_WorldMatrix);
	//D3DXMatrixTranslation(&m_WorldMatrix, 4.0, 0.0, 0.0);

	// 初始化顶点缓冲和顶点索引缓冲.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void AxisModelClass::Shutdown()
{
	// 释放顶点和索引缓冲.
	ShutdownBuffers();

	return;
}

void AxisModelClass::GetWorldMatrix(D3DXMATRIX& WorldMatrix)
{
	WorldMatrix = m_WorldMatrix;
}

void AxisModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 把顶点和索引缓冲放入图形管线，准备渲染.
	RenderBuffers(deviceContext);
	return;
}

int AxisModelClass::GetIndexCount()
{
	//返回索引顶点计数
	return m_indexCount;
}

bool AxisModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//首先，我们创建2个临时缓冲存放顶点和索引数据，以便后面使用。. 

	// 设置顶点缓冲大小为6
	m_vertexCount = 6;

	// 设置索引缓冲大小.
	m_indexCount = 6;

	// 创建顶点临时缓冲.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 创建索引缓冲.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 设置顶点数据.
	//x轴，红色
	vertices[0].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[0].color = RED;

	vertices[1].position = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	vertices[1].color = RED;

	//y轴，绿色
	vertices[2].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[2].color = GREEN;

	vertices[3].position = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	vertices[3].color = GREEN;

	//z轴，蓝色
	vertices[4].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[4].color = BLUE;

	vertices[5].position = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
	vertices[5].color = BLUE;


	// 设置索引缓冲数据.
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	// 设置顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 设置索引缓冲描述.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向存临时索引缓冲.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建索引缓冲.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 释放临时缓冲.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void AxisModelClass::ShutdownBuffers()
{
	// 释放顶点缓冲.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 释放索引缓冲
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void AxisModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// 设置顶点缓冲跨度和偏移.
	stride = sizeof(VertexType);
	offset = 0;

	//在input assemberl阶段绑定顶点缓冲，以便能够被渲染
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//在input assemberl阶段绑定索引缓冲，以便能够被渲染
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 设置体元语义，渲染线段，画出坐标轴
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}

