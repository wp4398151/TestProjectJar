#include "LightShaderClass.h"

static unsigned char s_lpShaderContentStr[] =
"cbuffer lightMaterialBuffer"
"{"
"	float4 lightPosition;		"	// 光源位置
"	float4 lightColor;			"	// 光源颜色
"	float4 globalAmbient;		"	// 环境光系数
"	float4 cameraPosition;		"	// 摄像机位置
"	float4 Ke;					"	// 材质自发光
"	float4 Ka;					"	// 材质环境光系数
"	float4 Kd;					"	// 材质漫反射系数
"	float4 Ks;					"	// 材质高光系数
"	float3 lightDir;			"	// 平行光方向
"	float shininess;			"	// 高光指数
"};";

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
	return true;
}
void LightShaderClass::Shutdown()
{

}

bool LightShaderClass::Render(ID3D11DeviceContext *pDeviceContext,
	int indexCount,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXVECTOR4 cameraPosition)
{
	return true;
}
bool LightShaderClass::RenderShader(ID3D11DeviceContext *pDeviceContext,
	int indexCount)
{
	return true;
}

bool LightShaderClass::SetShaderParam(
	ID3D11DeviceContext *pDeviceContext,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix,
	D3DXVECTOR4 cameraPosition)
{
	return true;
}

