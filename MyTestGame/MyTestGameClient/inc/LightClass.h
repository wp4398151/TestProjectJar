#pragma once
#include <D3D11.h>
#include <D3DX10math.h>

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass& rlightClass);
	~LightClass();

	void SetLightPosition(float x, float y,  float z, float w);
	void SetGlobalAmbient(float red, float green, float blue, float alpha);
	void SetDirection(float x, float y, float z);
	void SetLightColor(float red, float green, float blue, float alpha);
	void SetShininess(float shininess);

	D3DXVECTOR4 GetPosition();
	D3DXVECTOR4 GetGlobalAmbient();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetLightColor();
	float GetShininess();

private:
	D3DXVECTOR4 m_position;		//光源位置
	D3DXVECTOR4 m_globalAmbient; //全局环境光系数
	D3DXVECTOR3 m_direction;	//光源方向,平行光
	D3DXVECTOR4 m_lightColor;	//光源颜色
	float m_shininess;			//高光系数
};

