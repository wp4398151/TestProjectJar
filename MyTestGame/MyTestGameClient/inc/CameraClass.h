#pragma once

#include <D3DX10math.h>

class CameraClass
{
public:
	CameraClass(void);
	CameraClass(const CameraClass&);
	~CameraClass(void);
	void SetPosition(float, float, float);
	D3DXVECTOR3 GetPosition();
	void GetRight(D3DXVECTOR3& right);
	void GetUp(D3DXVECTOR3& up);
	void GetLook(D3DXVECTOR3& look);

	void strafe(float units); // 左右 
	void fly(float units);   // 上下 
	void walk(float units);   // 前后

	void pitch(float angle); // 旋转view坐标系right向量
	void yaw(float angle);   // 旋转up向量
	void roll(float angle);  // 旋转look向量

	//void Render();
	void GetViewMatrix(D3DXMATRIX& rViewMatrix);

	D3DXVECTOR3 m_PosVector;	// 摄像机的位置
private:

	D3DXVECTOR3 m_UpVector;		// 上方Vector
	D3DXVECTOR3 m_LookAtVetor;	// 朝向Vector
	D3DXVECTOR3 m_RightVetor;	// 朝向Vector
};

