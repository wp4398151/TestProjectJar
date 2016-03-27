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

	void strafe(float units); // ���� 
	void fly(float units);   // ���� 
	void walk(float units);   // ǰ��

	void pitch(float angle); // ��תview����ϵright����
	void yaw(float angle);   // ��תup����
	void roll(float angle);  // ��תlook����

	//void Render();
	void GetViewMatrix(D3DXMATRIX& rViewMatrix);

	D3DXVECTOR3 m_PosVector;	// �������λ��
private:

	D3DXVECTOR3 m_UpVector;		// �Ϸ�Vector
	D3DXVECTOR3 m_LookAtVetor;	// ����Vector
	D3DXVECTOR3 m_RightVetor;	// ����Vector
};

