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
"	                                                                "//世界坐标系中的顶点法向
"    float3 worldNomal = mul(input.normal, (float3x3)worldMatrix);                           "
"	worldNomal = normalize(worldNomal);                                                      "
"	                                                                     "//世界坐标系顶点位置
"    worldPosition = mul(input.position, worldMatrix);                                       "
"	float3 worldVertexPosition = worldPosition.xyz;                                          "
"	                                                                                         "
"	float3 emissive = float3(0.0, 0.0, 0.0);                                                                    "//自发射颜色 
"	                                                                             "
"    float3 ambient = float3(0.3, 0.3, 0.3);                                                    "//计算环境光
"	                                                                           "//计算漫反射光
"    float3 WorldLightDir = normalize(float3(-1.0, -1.0, 1.0));                                      "
"    float diffuseLight = max(dot(worldNomal, WorldLightDir), 0);                            "
"    float3 diffuse = diffuseLight;                                        "
"	                                                                               "//计算高光
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
//"	float4 lightPosition;		"	// 光源位置
//"	float4 lightColor;			"	// 光源颜色
//"	float4 globalAmbient;		"	// 环境光系数
//"	float4 cameraPosition;		"	// 摄像机位置
//"	float4 Ke;					"	// 材质自发光
//"	float4 Ka;					"	// 材质环境光系数
//"	float4 Kd;					"	// 材质漫反射系数
//"	float4 Ks;					"	// 材质高光系数
//"	float3 lightDirection;			"	// 平行光方向
//"	float shininess;			"	// 高光指数
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
//"	                                                                "//世界坐标系中的顶点法向
//"    float3 worldNomal = mul(input.normal, (float3x3)worldMatrix);                           "
//"	worldNomal = normalize(worldNomal);                                                      "
//"	                                                                     "//世界坐标系顶点位置
//"    worldPosition = mul(input.position, worldMatrix);                                       "
//"	float3 worldVertexPosition = worldPosition.xyz;                                          "
//"	                                                                                         "
//"	float4 emissive = Ke;                                                                    "//自发射颜色 
//"	                                                                             "
//"    float4 ambient = Ka * globalAmbient;                                                    "//计算环境光
//"	                                                                           "//计算漫反射光
//"    float3 WorldLightDir = -normalize(lightDirection);                                      "
//"    float diffuseLight = max(dot(worldNomal, WorldLightDir), 0);                            "
//"    float4 diffuse = Kd * lightColor * diffuseLight;                                        "
//"	                                                                               "//计算高光
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
	// 设置数据布局，以便在shader中使用.
	// 定义要和ModelClass中的顶点结构一致.
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
	// 得到layout中的元素数量
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	// 创建顶点输入布局.
	result = pDevice->CreateInputLayout(polygonLayout, numElements, 
		pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(),
		&m_pLayout);
	assert(SUCCEEDED(result));

	//释放顶点和像素缓冲.
	pVertexShaderBuffer->Release();
	pVertexShaderBuffer = 0;

	pPixelShaderBuffer->Release();
	pPixelShaderBuffer = 0;

	// 设置动态矩阵描述.
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
	// 得到错误信息buffer指针.
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	// 得到buffer的长度.
	bufferSize = errorMessage->GetBufferSize();
	errorMessage->Release();
	errorMessage = 0;
	return;
}

void LightShaderClass::Shutdown()
{
	// 释放光照材质缓冲.
	if (m_pLightMaterialBuffer)
	{
		m_pLightMaterialBuffer->Release();
		m_pLightMaterialBuffer = NULL;
	}

	// 释放常量缓冲
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = 0;
	}

	//释放顶点布局.
	if (m_pLayout)
	{
		m_pLayout->Release();
		m_pLayout = 0;
	}

	// 释放ps
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = 0;
	}

	// 释放vs
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
	// 设置shader参数.
	result = SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix, lightPosition, lightColor, globalAmbient,
		cameraPosition, Ke, Ka, Kd, Ks, lightDirection, shininess);
	assert(result);

	// 用指定shader渲染顶点缓冲.
	result = RenderShader(pDeviceContext, indexCount);
	assert(result);
	return result;
}
bool LightShaderClass::RenderShader(ID3D11DeviceContext *pDeviceContext,
	int indexCount)
{
	// 绑定顶点布局.
	pDeviceContext->IASetInputLayout(m_pLayout);

	// 设置渲染使用vs和ps.
	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	// 渲染三角形
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
	
	// 传入shader前，确保矩阵转置，这是D3D11的要求.
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

	// 锁定光照材质缓冲.
	result = pDeviceContext->Map(m_pLightMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	//得到常量缓冲数据指针
	pLightBufferBufferPtr = (LightMaterialBufferType*)mappedResource.pData;

	// 常量缓冲赋值.
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

	// 解锁常量缓冲
	pDeviceContext->Unmap(m_pLightMaterialBuffer, 0);

	// 设置缓冲索引为1，因为这是vs中的第二个常量缓冲，第一个为矩阵.
	bufferNumber = 1;

	// 设置光照材质常量缓冲.
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pLightMaterialBuffer);

	return true;
}

