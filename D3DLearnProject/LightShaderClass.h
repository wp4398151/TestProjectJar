#pragma once
#include <D3D11.h>
#include <D3DX10math.h>

#define NUM_LIGHTS 2	// ��Դ��

class LightShaderClass
{
public:
	LightShaderClass();
	~LightShaderClass();
	//��shader��const buffer�ṹһ�£���Ҫ��������Щshader������ֵ��
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	struct  LightMaterialBufferType
	{
		D3DXVECTOR4 lightPosition[NUM_LIGHTS]; //��Դλ��
		D3DXVECTOR4 lightColor[NUM_LIGHTS];   //��Դ��ɫ
		D3DXVECTOR4 globalAmbient[NUM_LIGHTS]; //��Դ�Ļ����ⷴ��ϵ��
		D3DXVECTOR4 cameraPosition; //�������λ��
		D3DXVECTOR4 Ke[NUM_LIGHTS];  //���ʵ��Է���
		D3DXVECTOR4 Ka[NUM_LIGHTS];  //���ʵĻ�����ϵ��
		D3DXVECTOR4 Kd[NUM_LIGHTS];  //���ʵ�������ϵ��
		D3DXVECTOR4 Ks[NUM_LIGHTS];  //���ʵĸ߹�ϵ��
		D3DXVECTOR4 attenuation[NUM_LIGHTS]; //˥��ϵ����x,y,z��Ӧ���������ԺͶ���ϵ��
		D3DXVECTOR4 spotattenuation[NUM_LIGHTS];//����spotlight��x��y�ֱ�洢�ں����cosֵ
		D3DXVECTOR3 lightDirection[NUM_LIGHTS]; //ƽ�йⷽ��
		float shininess[NUM_LIGHTS]; //�߹�ָ��
	};

public:
	bool Init(ID3D11Device *pDevice, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext *pDeviceContext,
		int indexCount,
		D3DXMATRIX worldMatrix,
		D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix,
		D3DXVECTOR4 cameraPosition);
private:
	bool RenderShader(ID3D11DeviceContext *pDeviceContext,
		int indexCount);

	bool SetShaderParam(
		ID3D11DeviceContext *pDeviceContext,
		D3DXMATRIX worldMatrix,
		D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix,
		D3DXVECTOR4 cameraPosition);

private:
	ID3D11InputLayout *m_pLayout;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;
	ID3D11Buffer *m_pConstantBuffer;		// ���þ���buf
	ID3D11Buffer *m_pLightMaterialBuffer;	// ���ղ���buf
};

