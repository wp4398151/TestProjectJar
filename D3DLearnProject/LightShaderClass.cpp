#include "LightShaderClass.h"

static unsigned char s_lpShaderContentStr[] =
"cbuffer lightMaterialBuffer"
"{"
"	float4 lightPosition;		"	// ��Դλ��
"	float4 lightColor;			"	// ��Դ��ɫ
"	float4 globalAmbient;		"	// ������ϵ��
"	float4 cameraPosition;		"	// �����λ��
"	float4 Ke;					"	// �����Է���
"	float4 Ka;					"	// ���ʻ�����ϵ��
"	float4 Kd;					"	// ����������ϵ��
"	float4 Ks;					"	// ���ʸ߹�ϵ��
"	float3 lightDir;			"	// ƽ�йⷽ��
"	float shininess;			"	// �߹�ָ��
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

