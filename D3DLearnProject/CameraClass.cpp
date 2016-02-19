#include "CameraClass.h"

CameraClass::CameraClass(void)
{
	m_PosVector = {0.0f, 0.0f, 0.0f};	// �������λ��
	m_UpVector = {0.0f, 1.0f, 0.0f};	// �Ϸ�Vector
	m_LookAtVetor = {0.0f, 0.0f, 1.0f};	// ����Vector
	m_RightVetor = {1.0f, 0.0f, 0.0f};	// ����Vector
}

CameraClass::CameraClass(const CameraClass& cameraClass)
{

}

CameraClass::~CameraClass(void)
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_PosVector.x = x;
	m_PosVector.y = y;
	m_PosVector.z = z;
	return;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return m_PosVector;
}

void CameraClass::GetRight(D3DXVECTOR3& right)
{
	right = m_RightVetor;
}

void CameraClass::GetUp(D3DXVECTOR3& up)
{
	up = m_UpVector;
}

void CameraClass::GetLook(D3DXVECTOR3& look)
{
	look = m_UpVector;
}

// ���� 
void CameraClass::strafe(float units)
{
	m_PosVector += units*m_RightVetor;
}

// ���� 
void CameraClass::fly(float units)
{
	// ��Ҫ��Up�����ƶ�
	m_PosVector += units*m_UpVector;
}

// ǰ��
void CameraClass::walk(float units)
{
	// ��Ҫ���ӵ�ķ����ƶ�
	m_PosVector += units*m_LookAtVetor;
}

// ��תview����ϵright����
void CameraClass::pitch(float angle)
{
	// �õ�����right����ת������
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_RightVetor, angle);

	// ������������Ӧ����ת
	D3DXVec3TransformCoord(&m_UpVector, &m_UpVector, &T);
	D3DXVec3TransformCoord(&m_LookAtVetor, &m_LookAtVetor, &T);
}

// ��תup����
void CameraClass::yaw(float angle)
{
	// �õ���������ת������
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_UpVector, angle);

	// ������������Ӧ����ת
	D3DXVec3TransformCoord(&m_RightVetor, &m_RightVetor, &T);
	D3DXVec3TransformCoord(&m_LookAtVetor, &m_LookAtVetor, &T);
}

// ��תlook����
void CameraClass::roll(float angle)
{
	// �õ���������ת������
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_LookAtVetor, angle);

	// ������������Ӧ����ת
	D3DXVec3TransformCoord(&m_RightVetor, &m_RightVetor, &T);
	D3DXVec3TransformCoord(&m_UpVector, &m_UpVector, &T);
}

//void CameraClass::Render()
//{
//	D3DXVECTOR3 up; // ��������Ϸ�
//	D3DXVECTOR3 position; // �������λ��
//	D3DXVECTOR3 lookAt; // �������Ŀ��λ��
//	float yaw = 0.0f;
//	float pitch = 0.0f;
//	float roll = 0.0f;
//
//	D3DXMATRIX rotationMatrix;	// ��ת����
//	
//	// ���ϵ�����
//	up.x = 0.0f;
//	up.y = 1.0f;
//	up.z = 0.0f;
//	
//	// �����������λ��
//	//position.x = m_positionX;
//	//position.y = m_positionY;
//	//position.z = m_positionZ;
//	
//	// ���������lookat�ķ���.
//	//lookAt.x = 0.0f;
//	//lookAt.y = 0.0f;
//	//lookAt.z = 1.0f;
//
//	D3DXVec3Normalize(&lookAt, &position);
//	lookAt = lookAt * (-1);
//
//	// �õ����ȵ�λ��ŷ����ת yaw (Y axis), pitch (X axis), �Լ� roll (Z axis) .
//	//pitch = m_rotationX * 0.0174532925f;
//	//yaw = m_rotationY * 0.0174532925f;
//	//roll = m_rotationZ * 0.0174532925f;
//
//	// �õ���ת����.
//	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
//
//	// �任lookat��up������һ��view�ܹ���ԭ�㱻��ȷ��ת.
//	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
//	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
//
//	// ƽ����ת��������λ��, ��Ϊλ��֮����������λ��
//	lookAt = position + lookAt;
//
//	// ����view����.
//	//D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
//}

void CameraClass::GetViewMatrix(D3DXMATRIX& rViewMatrix)
{
	D3DXVec3Normalize(&m_LookAtVetor, &m_LookAtVetor);

	D3DXVec3Cross(&m_UpVector, &m_RightVetor, &m_LookAtVetor);
	D3DXVec3Normalize(&m_UpVector, &m_UpVector);
	
	D3DXVec3Cross(&m_RightVetor, &m_LookAtVetor, &m_UpVector);
	D3DXVec3Normalize(&m_RightVetor, &m_RightVetor);

	//// ����view����: 
	//float x = -D3DXVec3Dot(&m_RightVetor, &m_PosVector);
	//float y = -D3DXVec3Dot(&m_UpVector, &m_PosVector);
	//float z = -D3DXVec3Dot(&m_LookAtVetor, &m_PosVector);
	D3DXVECTOR3 posLookAt = m_PosVector + m_LookAtVetor;
	// ����view����.
	//D3DXMatrixLookAtLH(&rViewMatrix, &m_PosVector, &m_PosVector, &m_UpVector);
	D3DXMatrixLookAtLH(&rViewMatrix, &m_PosVector, &posLookAt , &m_UpVector);
	
	return;
}