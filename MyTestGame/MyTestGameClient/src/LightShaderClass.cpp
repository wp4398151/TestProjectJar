#include "LightShaderClass.h"
#include <D3DX11async.h>
#include <assert.h>

static const char s_lpVertexShaderContentStr[] =
"cbuffer MatrixBuffer           "
"{                              "
"	matrix worldMatrix;         "
"	matrix viewMatrix;          "
"	matrix projectionMatrix;    "
"};                             "
"cbuffer lightMaterialBuffer"
"{"
"	float3 cameraPosition;"
"	float padding;"
"};"
"struct VertexInputType         "
"{                              "
"	float4 position : POSITION; "
"	float3 normal : NORMAL;     "
"};                             "
"                               "
"struct PixelInputType          "
"{                              "
"	float4 position : SV_POSITION; "
"	float4 color : COLOR;     "
"};                             "
"PixelInputType LightVertexShader(VertexInputType input)                                     "
"{                                                                                           "
"    PixelInputType output;                                                                  "
"    float4 worldPosition;                                                                   "
"	input.position.w = 1.0f;                                                                 "
"	"// Calculate the position of the vertex against the world, view, and projection matrices.
"    output.position = mul(input.position, worldMatrix);                                     "
"    output.position = mul(output.position, viewMatrix);                                     "
"    output.position = mul(output.position, projectionMatrix);                               "
"	                                                                "//��������ϵ�еĶ��㷨��
"    float3 worldNomal = mul(input.normal, (float3x3)worldMatrix);                           "
"	worldNomal = normalize(worldNomal);                                                      "
"	                                                                     "//��������ϵ����λ��
"    worldPosition = mul(input.position, worldMatrix);                                       "
"	float3 worldVertexPosition = worldPosition.xyz;                                          "
"	                                                                                         "
"	float3 emissive = float3(0.0, 0.0, 0.0);                                                                    "//�Է�����ɫ 
"	                                                                             "
"    float3 ambient = float3(0.3, 0.3, 0.3);                                                    "//���㻷����
"	                                                                           "//�����������
"    float3 WorldLightDir = normalize(float3(-1.0, -1.0, 1.0));                                      "
"    float diffuseLight = max(dot(worldNomal, WorldLightDir), 0);                            "
"    float3 diffuse = diffuseLight;                                        "
"	                                                                               "//����߹�
"    float3 distCameraToVertex = normalize(cameraPosition - worldVertexPosition);        "
"    float3 hilightDir = normalize(WorldLightDir + distCameraToVertex);                      "
"    float specularLight = pow(max(dot(worldNomal, hilightDir), 0), 5.0);              "
"	                                                                                         "
"    if (diffuseLight <= 0)                                                                  "
"	      specularLight = 0;                                                                 "
"    float3 specular = specularLight;                                      "
"                                                                                            "
"	 output.color.xyz = emissive + ambient + diffuse + specular;                                  "
"    output.color.w = 1.0f;                                                                                        "
"    return output;                                                                          "
"}\r\n";

//"cbuffer lightMaterialBuffer	"
//"{"
//"	float4 lightPosition;		"	// ��Դλ��
//"	float4 lightColor;			"	// ��Դ��ɫ
//"	float4 globalAmbient;		"	// ������ϵ��
//"	float4 cameraPosition;		"	// �����λ��
//"	float4 Ke;					"	// �����Է���
//"	float4 Ka;					"	// ���ʻ�����ϵ��
//"	float4 Kd;					"	// ����������ϵ��
//"	float4 Ks;					"	// ���ʸ߹�ϵ��
//"	float3 lightDirection;			"	// ƽ�йⷽ��
//"	float shininess;			"	// �߹�ָ��
//"};                             "
//"struct VertexInputType         "
//"{                              "
//"	float4 position : POSITION; "
//"	float3 normal : NORMAL;     "
//"};                             "
//"                               "
//"struct PixelInputType          "
//"{                              "
//"	float4 position : SV_POSITION; "
//"	float4 color : COLOR;     "
//"};                             "
//"PixelInputType LightVertexShader(VertexInputType input)                                     "
//"{                                                                                           "
//"    PixelInputType output;                                                                  "
//"    float4 worldPosition;                                                                   "
//"	input.position.w = 1.0f;                                                                 "
//"	"// Calculate the position of the vertex against the world, view, and projection matrices.
//"    output.position = mul(input.position, worldMatrix);                                     "
//"    output.position = mul(output.position, viewMatrix);                                     "
//"    output.position = mul(output.position, projectionMatrix);                               "
//"	                                                                "//��������ϵ�еĶ��㷨��
//"    float3 worldNomal = mul(input.normal, (float3x3)worldMatrix);                           "
//"	worldNomal = normalize(worldNomal);                                                      "
//"	                                                                     "//��������ϵ����λ��
//"    worldPosition = mul(input.position, worldMatrix);                                       "
//"	float3 worldVertexPosition = worldPosition.xyz;                                          "
//"	                                                                                         "
//"	float4 emissive = Ke;                                                                    "//�Է�����ɫ 
//"	                                                                             "
//"    float4 ambient = Ka * globalAmbient;                                                    "//���㻷����
//"	                                                                           "//�����������
//"    float3 WorldLightDir = -normalize(lightDirection);                                      "
//"    float diffuseLight = max(dot(worldNomal, WorldLightDir), 0);                            "
//"    float4 diffuse = Kd * lightColor * diffuseLight;                                        "
//"	                                                                               "//����߹�
//"    float3 distCameraToVertex = normalize(cameraPosition.xyz - worldVertexPosition);        "
//"    float3 hilightDir = normalize(WorldLightDir + distCameraToVertex);                      "
//"    float specularLight = pow(max(dot(worldNomal, hilightDir), 0), shininess);              "
//"	                                                                                         "
//"    if (diffuseLight <= 0)                                                                  "
//"	      specularLight = 0;                                                                 "
//"    float4 specular = Ks * lightColor * specularLight;                                      "
//"                                                                                            "
//"	output.color = emissive + ambient + diffuse + specular;                                  "
//"                                                                                            "
//"    return output;                                                                          "
//"}\r\n";


static const char s_lpPixelShaderContentStr[] =
"struct PixelInputType                                     "
"{                                                         "
"    float4 position : SV_POSITION;                        "
"    float4 color : COLOR;                                 "
"};                                                        "
"                                                          "
"float4 LightPixelShader(PixelInputType input) : SV_TARGET "
"{                                                         "
"    return input.color;                                   "
"}                                                         "
"";

LightShaderClass::LightShaderClass()
{
	m_pLayout = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pConstantBuffer = NULL;
	m_pLightMaterialBuffer = NULL;
}

LightShaderClass::~LightShaderClass()
{

}

bool LightShaderClass::Init(ID3D11Device *pDevice, HWND hwnd)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorMessage = NULL;
	ID3D10Blob* pVertexShaderBuffer = NULL;
	ID3D10Blob* pPixelShaderBuffer = NULL;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements = 0;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightmaterialBufferDesc;

	result = D3DX11CompileFromMemory(s_lpVertexShaderContentStr,
		sizeof(s_lpVertexShaderContentStr),
		NULL, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &pVertexShaderBuffer, &pErrorMessage, NULL);

	//OutputShaderErrorMessage(pErrorMessage);
	assert(SUCCEEDED(result));

	result = D3DX11CompileFromMemory(s_lpPixelShaderContentStr,
		sizeof(s_lpPixelShaderContentStr),
		NULL, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &pPixelShaderBuffer, &pErrorMessage, NULL);
	assert(SUCCEEDED(result));

	result = pDevice->CreateVertexShader(
		pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(),
		NULL, &m_pVertexShader);
	assert(SUCCEEDED(result));
	result = pDevice->CreatePixelShader(
		pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(),
		NULL, &m_pPixelShader);
	assert(SUCCEEDED(result));
	// �������ݲ��֣��Ա���shader��ʹ��.
	// ����Ҫ��ModelClass�еĶ���ṹһ��.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	// �õ�layout�е�Ԫ������
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	// �����������벼��.
	result = pDevice->CreateInputLayout(polygonLayout, numElements, 
		pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(),
		&m_pLayout);
	assert(SUCCEEDED(result));

	//�ͷŶ�������ػ���.
	pVertexShaderBuffer->Release();
	pVertexShaderBuffer = 0;

	pPixelShaderBuffer->Release();
	pPixelShaderBuffer = 0;

	// ���ö�̬��������.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pConstantBuffer);
	assert(SUCCEEDED(result));
	// light 
	lightmaterialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightmaterialBufferDesc.ByteWidth = sizeof(LightMaterialBufferType);
	lightmaterialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightmaterialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightmaterialBufferDesc.MiscFlags = 0;
	lightmaterialBufferDesc.StructureByteStride = 0;
	result = pDevice->CreateBuffer(&lightmaterialBufferDesc, NULL, &m_pLightMaterialBuffer);
	assert(SUCCEEDED(result));

	return true;
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	// �õ�������Ϣbufferָ��.
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	// �õ�buffer�ĳ���.
	bufferSize = errorMessage->GetBufferSize();
	errorMessage->Release();
	errorMessage = 0;
	return;
}

void LightShaderClass::Shutdown()
{
	// �ͷŹ��ղ��ʻ���.
	if (m_pLightMaterialBuffer)
	{
		m_pLightMaterialBuffer->Release();
		m_pLightMaterialBuffer = NULL;
	}

	// �ͷų�������
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = 0;
	}

	//�ͷŶ��㲼��.
	if (m_pLayout)
	{
		m_pLayout->Release();
		m_pLayout = 0;
	}

	// �ͷ�ps
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = 0;
	}

	// �ͷ�vs
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = 0;
	}
}

bool LightShaderClass::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, D3DXVECTOR4 lightPosition, D3DXVECTOR4 lightColor, D3DXVECTOR4 globalAmbient,
	D3DXVECTOR4 cameraPosition, D3DXVECTOR4 Ke, D3DXVECTOR4 Ka, D3DXVECTOR4 Kd, D3DXVECTOR4 Ks,
	D3DXVECTOR3 lightDirection, float shininess)
{
	bool result;
	// ����shader����.
	result = SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix, lightPosition, lightColor, globalAmbient,
		cameraPosition, Ke, Ka, Kd, Ks, lightDirection, shininess);
	assert(result);

	// ��ָ��shader��Ⱦ���㻺��.
	result = RenderShader(pDeviceContext, indexCount);
	assert(result);
	return result;
}
bool LightShaderClass::RenderShader(ID3D11DeviceContext *pDeviceContext,
	int indexCount)
{
	// �󶨶��㲼��.
	pDeviceContext->IASetInputLayout(m_pLayout);

	// ������Ⱦʹ��vs��ps.
	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	// ��Ⱦ������
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return true;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, D3DXVECTOR4 lightPosition, D3DXVECTOR4 lightColor, D3DXVECTOR4 globalAmbient,
	D3DXVECTOR4 cameraPosition, D3DXVECTOR4 Ke, D3DXVECTOR4 Ka, D3DXVECTOR4 Kd, D3DXVECTOR4 Ks,
	D3DXVECTOR3 lightDirection, float shininess)
{
	HRESULT result = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* pMatrixBufferPtr = NULL;
	LightMaterialBufferType* pLightBufferBufferPtr = NULL;
	
	// ����shaderǰ��ȷ������ת�ã�����D3D11��Ҫ��.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = pDeviceContext->Map(m_pConstantBuffer, 0, 
		D3D11_MAP_WRITE_DISCARD, 
		0, &mappedResource);
	assert(SUCCEEDED(result));

	pMatrixBufferPtr = (MatrixBufferType*)mappedResource.pData;
	pMatrixBufferPtr->world = worldMatrix;
	pMatrixBufferPtr->view = viewMatrix;
	pMatrixBufferPtr->projection = projectionMatrix;
	
	pDeviceContext->Unmap(m_pConstantBuffer, 0);
	
	// index of cbuffer 
	bufferNumber = 0;
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pConstantBuffer);

	// �������ղ��ʻ���.
	result = pDeviceContext->Map(m_pLightMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	//�õ�������������ָ��
	pLightBufferBufferPtr = (LightMaterialBufferType*)mappedResource.pData;

	// �������帳ֵ.
	//pLightBufferBufferPtr->lightPosition = lightPosition;
	//pLightBufferBufferPtr->lightColor = lightColor;
	pLightBufferBufferPtr->cameraPosition.x = cameraPosition.x;
	pLightBufferBufferPtr->cameraPosition.y = cameraPosition.y;
	pLightBufferBufferPtr->cameraPosition.z = cameraPosition.z;
	//pLightBufferBufferPtr->globalAmbient = globalAmbient;
	//pLightBufferBufferPtr->Ke = Ke;
	//pLightBufferBufferPtr->Ka = Ke;
	//pLightBufferBufferPtr->Kd = Kd;
	//pLightBufferBufferPtr->Ks = Ks;
	//pLightBufferBufferPtr->lightDirection = lightDirection;
	//pLightBufferBufferPtr->shininess = shininess;

	// ������������
	pDeviceContext->Unmap(m_pLightMaterialBuffer, 0);

	// ���û�������Ϊ1����Ϊ����vs�еĵڶ����������壬��һ��Ϊ����.
	bufferNumber = 1;

	// ���ù��ղ��ʳ�������.
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pLightMaterialBuffer);

	return true;
}

