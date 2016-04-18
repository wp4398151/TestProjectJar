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

	// ��ʼ�����㻺��Ͷ�����������.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void AxisModelClass::Shutdown()
{
	// �ͷŶ������������.
	ShutdownBuffers();

	return;
}

void AxisModelClass::GetWorldMatrix(D3DXMATRIX& WorldMatrix)
{
	WorldMatrix = m_WorldMatrix;
}

void AxisModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �Ѷ���������������ͼ�ι��ߣ�׼����Ⱦ.
	RenderBuffers(deviceContext);
	return;
}

int AxisModelClass::GetIndexCount()
{
	//���������������
	return m_indexCount;
}

bool AxisModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//���ȣ����Ǵ���2����ʱ�����Ŷ�����������ݣ��Ա����ʹ�á�. 

	// ���ö��㻺���СΪ6
	m_vertexCount = 6;

	// �������������С.
	m_indexCount = 6;

	// ����������ʱ����.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// ������������.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ���ö�������.
	//x�ᣬ��ɫ
	vertices[0].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[0].color = RED;

	vertices[1].position = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	vertices[1].color = RED;

	//y�ᣬ��ɫ
	vertices[2].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[2].color = GREEN;

	vertices[3].position = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	vertices[3].color = GREEN;

	//z�ᣬ��ɫ
	vertices[4].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[4].color = BLUE;

	vertices[5].position = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
	vertices[5].color = BLUE;


	// ����������������.
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	// ���ö��㻺������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������㻺��.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ����������������.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ָ�����ʱ��������.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// ������������.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ͷ���ʱ����.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void AxisModelClass::ShutdownBuffers()
{
	// �ͷŶ��㻺��.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// �ͷ���������
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

	// ���ö��㻺���Ⱥ�ƫ��.
	stride = sizeof(VertexType);
	offset = 0;

	//��input assemberl�׶ΰ󶨶��㻺�壬�Ա��ܹ�����Ⱦ
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//��input assemberl�׶ΰ��������壬�Ա��ܹ�����Ⱦ
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ������Ԫ���壬��Ⱦ�߶Σ�����������
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}

