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

// ��Shader ֱ��д���ַ�����,���ù���Shader�ļ�
static unsigned char s_lpShaderContentStr[] =
"cbuffer MatrixBuffer{			"
"	matrix worldMatrix;			"
"	matrix viewMatrix;				"
"	matrix projectionMatrix;		"
"};								"
"struct VertexInputType{		"
"	float4 position : POSITION;	"
"	float4 color : COLOR;			"
"};								"
"struct PixelInputType{			"
"	float4 position : SV_POSITION;			"
"	float4 color : COLOR;			"
"};								"
"PixelInputType ColorVertexShader(PixelInputType input){"
"	PixelInputType output;		"
"	input.position.w = 1.0f;	"
"	output.position = mul(input.position, worldMatrix);			"
"	output.position = mul(input.position, viewMatrix);			"
"	output.position = mul(input.position, projectionMatrix);	"
"	output.color = input.color;	"
"	return output;				"
"}								"
"float4 ColorPixelShader(PixelInputType input) : SV_TAGET{"
"	return input.color;"
"}";

bool ShaderClass::Render(ID3D11DeviceContext *pDeviceContext,
			int indexCount,
			D3DXMATRIX worldMatrix,
			D3DXMATRIX viewMatrix,
			D3DXMATRIX projectionMatrix)
{
	//pDeviceContext->VSSetConstantBuffers(,ID3D11Buffer);
	bool res = true;
	res = SetShaderParam(pDeviceContext,
		worldMatrix, viewMatrix, projectionMatrix);
	assert(!res);
	RenderShader(pDeviceContext, indexCount);
	return res;
}


void ShaderClass::Shutdown()
{
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
	ID3D10Blob *pErr = NULL; // ��ű���shader�Ĵ�����Ϣ
	ID3D10Blob *pVertexShaderBuffer = NULL; // ���������VS�ֽ�
	ID3D10Blob *pPixelShaderBuffer = NULL; // ���������PS�ֽ�
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2]; // ����
	unsigned int numElem = sizeof(polygonLayout)/sizeof(polygonLayout[0]); // һ�����еĳ�Ա��
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };

	// ����VS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpShaderContentStr,
		sizeof(s_lpShaderContentStr),
		NULL, NULL, NULL,
		"ColorVertexShader",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuffer, &pErr, NULL);
	assert(SUCCEEDED(res));
	
	// ����PS
	res = D3DX11CompileFromMemory((LPCSTR)s_lpShaderContentStr,
		sizeof(s_lpShaderContentStr),
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
	
	// ����Content ����Buffer	
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
	
	return true;
}

bool ShaderClass::RenderShader(ID3D11DeviceContext *pDeviceContext,
					int indexCount)
{
	pDeviceContext->IASetInputLayout(m_pLayout);
	
	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
	return true;
}

bool ShaderClass::SetShaderParam(ID3D11DeviceContext *pDeviceContext,
								D3DXMATRIX worldMatrix,
								D3DXMATRIX viewMatrix,
								D3DXMATRIX projectionMatrix)
{
	HRESULT res = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *pBufferPtr = NULL;
	unsigned int bufferNum = NULL;
	
	// ����֮ǰ��Ҫת��	
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
	// ������VS Constant Buffer
	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &m_pConstantBuffer);
	return true;
}