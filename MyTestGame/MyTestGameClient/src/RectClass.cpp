#include "RectClass.h"
#include <assert.h>
#include <D3DX11async.h>

struct MatrixBufferType
{
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
};




static char s_RectShaderStr[] =
"Texture2D shaderTexture;		"
"SamplerState SampleType;		"
"cbuffer MatrixBuffer{			"
"	matrix worldMatrix;			"
"	matrix viewMatrix;			"
"	matrix projectionMatrix;	"
"};								"
"struct VertexInputType{		"
"	float4 position : POSITION;	"		// λ��
"	float4 color : COLOR;		"		// ��ɫ
"	float2 texcoord : TEXCOORD0;"		// ��ͼ����
"};								"
"struct PixelInputType{			"
"	float4 position : SV_POSITION;	"
"	float4 color : COLOR;			"
"	float2 texcoord : TEXCOORD0;"		// ��ͼ����
"};								"
"PixelInputType ColorVertexShader(VertexInputType input){		"
"	PixelInputType output;										"
"	input.position.w = 1.0f;									"
"	output.position = mul(input.position, worldMatrix);			"
"	output.position = mul(output.position, viewMatrix);			"
"	output.position = mul(output.position, projectionMatrix);	"
"	output.texcoord = input.texcoord;							"
"	output.color = input.color;									"
"	return output;												"
"}								"
"float4 ColorPixelShader(PixelInputType input) : SV_TARGET{"
"	float4 texturecolor;									"
"	texturecolor = shaderTexture.Sample(SampleType, input.texcoord);"
"	return texturecolor;											"
"}";

RectClass::RectClass()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_IndexCount = 6;
	m_vertexCount = 4;

	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pLayout = NULL;
	m_pContantBuffer = NULL;
}

RectClass::~RectClass()
{

}

bool RectClass::Initialize(ID3D11Device* pDevice, int rectRidus, float z)
{
	float radius = rectRidus;
	float negRadius = -1 * radius;
	RectClass::VertexType s_RectClassVertexs[] = {
			{ D3DXVECTOR3(negRadius, radius, z), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f) },
			{ D3DXVECTOR3(radius, radius, z), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f) },
			{ D3DXVECTOR3(negRadius, negRadius, z), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f) },
			{ D3DXVECTOR3(radius, negRadius, z), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f) },
	};
	int s_RectClassIndexs[] = {0,1,2,2,1,3};

	D3D11_BUFFER_DESC vertexBufferDesc;	// ���㻺�������
	D3D11_BUFFER_DESC indexBufferDesc; // �����������������
	D3D11_SUBRESOURCE_DATA vertexData; // ������Ҫ���ʵ���Դ
	D3D11_SUBRESOURCE_DATA indexData; // ����������Ҫ���ʵ���Դ
	HRESULT res = S_OK;

	// ���ö��㻺������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(s_RectClassVertexs);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ����Դ�󶨵����㣬�����߷���
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����.
	vertexData.pSysMem = s_RectClassVertexs; // �������εĶ������ݷ��붥�㻺����
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������㻺��.
	res = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	assert(SUCCEEDED(res));

	// ���ö���������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(s_RectClassIndexs);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // ����Դ�󶨵����������������߷���
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����.
	indexData.pSysMem = s_RectClassIndexs; // �������εĶ������ݷ��붥�㻺����
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �������㻺��.
	res = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	assert(SUCCEEDED(res));

	// ��ʼ��Shader
	ID3D10Blob *pErr = NULL; // ��ű���shader�Ĵ�����Ϣ
	ID3D10Blob *pVertexShaderBuffer = NULL; // ���������VS�ֽ�
	ID3D10Blob *pPixelShaderBuffer = NULL; // ���������PS�ֽ�
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3]; // ����
	unsigned int numElem = sizeof(polygonLayout) / sizeof(polygonLayout[0]); // һ�����еĳ�Ա��
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };

	// ����VS
	res = D3DX11CompileFromMemory((LPCSTR)s_RectShaderStr,
		sizeof(s_RectShaderStr),
		NULL, NULL, NULL,
		"ColorVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));

	// ����PS
	res = D3DX11CompileFromMemory((LPCSTR)s_RectShaderStr,
		sizeof(s_RectShaderStr),
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

	polygonLayout[2].SemanticName = "TEXCOORD";	// ����
	polygonLayout[2].SemanticIndex = 0;	// ����
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;	// ����
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

	// ����Content ����Buffer	
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

void RectClass::Shutdown()
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
						D3DXMATRIX projectionMatrix,
							ID3D11ShaderResourceView* pTextureView, 
								ID3D11SamplerState* pSampleState)

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

	// ��ȾShader
	HRESULT res = S_OK;
	// ����֮ǰ��Ҫת��	
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

	// ������VS Constant Buffer
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pContantBuffer);
	// ��������
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureView);

	pDeviceContext->IASetInputLayout(m_pLayout);
	// ����VS Shader ��PS Shader
	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
	
	// ���ò���״̬
	pDeviceContext->PSSetSamplers(0, 1, &pSampleState);
	
	pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);
}