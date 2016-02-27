#include "LightClass.h"


LightClass::LightClass()
{
	m_position = D3DXVECTOR4(1.0, 0.0, 1.0, 1.0);
	m_globalAmbient = D3DXVECTOR4(1.0, 0.0, 1.0, 1.0);
	m_direction = D3DXVECTOR3(-1.0, -1.0, 1.0);
	m_lightColor = D3DXVECTOR4(1.0, 1.0, 1.0, 1.0);
	m_shininess = 5.0f;
}

LightClass::LightClass(const LightClass& rlightClass)
{

}

LightClass::~LightClass()
{
}

void  LightClass::SetLightPosition(float x, float y, float z, float w)
{
	m_position = D3DXVECTOR4(x, y, z, w);
}

//设置全局环境光系数
void LightClass::SetGlobalAmbient(float red, float green, float blue, float alpha)
{
	m_globalAmbient = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

//设置平行光方向
void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}

//设置光源颜色
void LightClass::SetLightColor(float red, float green, float blue, float alpha)
{
	m_lightColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

//设置全局环境光系数
void LightClass::SetShininess(float shininess)
{
	m_shininess = shininess;
	return;
}

//得到光源位置
D3DXVECTOR4 LightClass::GetPosition()
{
	return m_position;
}

//得到全局环境光系数
D3DXVECTOR4 LightClass::GetGlobalAmbient()
{
	return m_globalAmbient;
}

//得到平行光方向
D3DXVECTOR3 LightClass::GetDirection()
{
	return m_direction;
}

//得到光源颜色
D3DXVECTOR4 LightClass::GetLightColor()
{
	return m_lightColor;
}

//得到全局环境光系数
float LightClass::GetShininess()
{
	return m_shininess;
}
