#pragma once
#include <D3D11.h>
#include <D3DX10math.h>

#define NUM_LIGHTS 2	// 光源数

class LightShaderClass
{
public:
	LightShaderClass();
	~LightShaderClass();
	//和shader中const buffer结构一致，主要用来给这些shader参数赋值。
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	struct  LightMaterialBufferType
	{
		D3DXVECTOR4 lightPosition[NUM_LIGHTS]; //光源位置
		D3DXVECTOR4 lightColor[NUM_LIGHTS];   //光源颜色
		D3DXVECTOR4 globalAmbient[NUM_LIGHTS]; //光源的环境光反射系数
		D3DXVECTOR4 cameraPosition; //摄像机的位置
		D3DXVECTOR4 Ke[NUM_LIGHTS];  //材质的自发光
		D3DXVECTOR4 Ka[NUM_LIGHTS];  //材质的环境光系数
		D3DXVECTOR4 Kd[NUM_LIGHTS];  //材质的漫反射系数
		D3DXVECTOR4 Ks[NUM_LIGHTS];  //材质的高光系数
		D3DXVECTOR4 attenuation[NUM_LIGHTS]; //衰减系数，x,y,z对应常量，线性和二次系数
		D3DXVECTOR4 spotattenuation[NUM_LIGHTS];//对于spotlight，x，y分别存储内和外角cos值
		D3DXVECTOR3 lightDirection[NUM_LIGHTS]; //平行光方向
		float shininess[NUM_LIGHTS]; //高光指数
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
	ID3D11Buffer *m_pConstantBuffer;		// 放置矩阵buf
	ID3D11Buffer *m_pLightMaterialBuffer;	// 光照参数buf
};

