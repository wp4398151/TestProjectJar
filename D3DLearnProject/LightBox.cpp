#include "LightBox.h"
#include <assert.h>

// ����Ľṹ
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
	0,  // ǰ�� 
	1,
	2,
	0,
	2,
	3,

	4,  // ���� 
	6,
	5,
	4,
	7,
	6,

	4,  // ���� 
	5,
	1,
	4,
	1,
	0,

	3,  //���� 
	2,
	6,
	3,
	6,
	7,

	1,  // ���� 
	5,
	6,
	1,
	6,
	2,

	4, // ���� 
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
	// ��ǰ���õ�������
	WorldMatrix = m_WorldMatrix;
}

bool LightBox::InitializeBuffers(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC vertexBufferDesc;	// ���㻺�������
	D3D11_BUFFER_DESC indexBufferDesc; // �����������������
	D3D11_SUBRESOURCE_DATA vertexData; // ������Ҫ���ʵ���Դ
	D3D11_SUBRESOURCE_DATA indexData; // ����������Ҫ���ʵ���Դ
	HRESULT result = true;

	// ���ö��㻺������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ����Դ�󶨵����㣬�����߷���
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����.
	vertexData.pSysMem = g_LightBoxVertices; // �������εĶ������ݷ��붥�㻺����
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������㻺��.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(result));

	// ���ö���������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(g_LightBoxIndexVertice[0]) *m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // ����Դ�󶨵����������������߷���
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����.
	indexData.pSysMem = g_LightBoxIndexVertice; // �������εĶ������ݷ��붥�㻺����
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �������㻺��.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	assert(SUCCEEDED(result));
	return true;
}

void LightBox::ShutdownBuffers()
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

void LightBox::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
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