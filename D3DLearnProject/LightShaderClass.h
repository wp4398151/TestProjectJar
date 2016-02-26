#pragma once
#include <D3D11.h>
#include <D3DX10math.h>

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
		D3DXVECTOR4 lightPosition; //��Դλ��
		D3DXVECTOR4 lightColor;   //��Դ��ɫ
		D3DXVECTOR4 globalAmbient; //��Դ�Ļ����ⷴ��ϵ��
		D3DXVECTOR4 cameraPosition; //�������λ��
		D3DXVECTOR4 Ke;  //���ʵ��Է���
		D3DXVECTOR4 Ka;  //���ʵĻ�����ϵ��
		D3DXVECTOR4 Kd;  //���ʵ�������ϵ��
		D3DXVECTOR4 Ks;  //���ʵĸ߹�ϵ��
		D3DXVECTOR4 attenuation; //˥��ϵ����x,y,z��Ӧ���������ԺͶ���ϵ��
		D3DXVECTOR4 spotattenuation;//����spotlight��x��y�ֱ�洢�ں����cosֵ
		D3DXVECTOR3 lightDirection; //ƽ�йⷽ��
		float shininess; //�߹�ָ��
	};

public:
	bool Init(ID3D11Device *pDevice, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, D3DXVECTOR4 lightPosition, D3DXVECTOR4 lightColor, D3DXVECTOR4 globalAmbient,
		D3DXVECTOR4 cameraPosition, D3DXVECTOR4 Ke, D3DXVECTOR4 Ka, D3DXVECTOR4 Kd, D3DXVECTOR4 Ks,
		D3DXVECTOR3 lightDirection, float shininess);

private:
	bool RenderShader(ID3D11DeviceContext *pDeviceContext,
		int indexCount);
	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, D3DXVECTOR4 lightPosition, D3DXVECTOR4 lightColor, D3DXVECTOR4 globalAmbient,
		D3DXVECTOR4 cameraPosition, D3DXVECTOR4 Ke, D3DXVECTOR4 Ka, D3DXVECTOR4 Kd, D3DXVECTOR4 Ks,
		D3DXVECTOR3 lightDirection, float shininess);

	void OutputShaderErrorMessage(ID3D10Blob* errorMessage);

private:
	ID3D11InputLayout *m_pLayout;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;
	ID3D11Buffer *m_pConstantBuffer;		// ���þ���buf
	ID3D11Buffer *m_pLightMaterialBuffer;	// ���ղ���buf
};

