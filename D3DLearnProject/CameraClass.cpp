#include "CameraClass.h"

CameraClass::CameraClass(void)
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& cameraClass)
{

}

CameraClass::~CameraClass(void)
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}
void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}

D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	D3DXVECTOR3 up; // ��������Ϸ�
	D3DXVECTOR3 position; // �������λ��
	D3DXVECTOR3 lookAt; // �������Ŀ��λ��
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;

	D3DXMATRIX rotationMatrix;	// ��ת����
	
	// ���ϵ�����
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	
	// �����������λ��
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	
	// ���������lookat�ķ���.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// �õ����ȵ�λ��ŷ����ת yaw (Y axis), pitch (X axis), �Լ� roll (Z axis) .
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// �õ���ת����.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// �任lookat��up������һ��view�ܹ���ԭ�㱻��ȷ��ת.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// ƽ����ת��������λ��, ��Ϊλ��֮����������λ��
	lookAt = position + lookAt;

	// ����view����.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}