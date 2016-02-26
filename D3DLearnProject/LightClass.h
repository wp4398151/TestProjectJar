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
	D3DXVECTOR4 m_position;		//��Դλ��
	D3DXVECTOR4 m_globalAmbient; //ȫ�ֻ�����ϵ��
	D3DXVECTOR3 m_direction;	//��Դ����,ƽ�й�
	D3DXVECTOR4 m_lightColor;	//��Դ��ɫ
	float m_shininess;			//�߹�ϵ��
};

