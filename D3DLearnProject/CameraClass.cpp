#include "CameraClass.h"

CameraClass::CameraClass(void)
{
	m_PosVector = {0.0f, 0.0f, 0.0f};	// 摄像机的位置
	m_UpVector = {0.0f, 1.0f, 0.0f};	// 上方Vector
	m_LookAtVetor = {0.0f, 0.0f, 1.0f};	// 朝向Vector
	m_RightVetor = {1.0f, 0.0f, 0.0f};	// 朝向Vector
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

// 左右 
void CameraClass::strafe(float units)
{
	m_PosVector += units*m_RightVetor;
}

// 上下 
void CameraClass::fly(float units)
{
	// 需要向Up方向移动
	m_PosVector += units*m_UpVector;
}

// 前后
void CameraClass::walk(float units)
{
	// 需要向视点的方向移动
	m_PosVector += units*m_LookAtVetor;
}

// 旋转view坐标系right向量
void CameraClass::pitch(float angle)
{
	// 得到沿着right轴旋转的向量
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_RightVetor, angle);

	// 将另外两个轴应用旋转
	D3DXVec3TransformCoord(&m_UpVector, &m_UpVector, &T);
	D3DXVec3TransformCoord(&m_LookAtVetor, &m_LookAtVetor, &T);
}

// 旋转up向量
void CameraClass::yaw(float angle)
{
	// 得到沿着轴旋转的向量
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_UpVector, angle);

	// 将另外两个轴应用旋转
	D3DXVec3TransformCoord(&m_RightVetor, &m_RightVetor, &T);
	D3DXVec3TransformCoord(&m_LookAtVetor, &m_LookAtVetor, &T);
}

// 旋转look向量
void CameraClass::roll(float angle)
{
	// 得到沿着轴旋转的向量
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_LookAtVetor, angle);

	// 将另外两个轴应用旋转
	D3DXVec3TransformCoord(&m_RightVetor, &m_RightVetor, &T);
	D3DXVec3TransformCoord(&m_UpVector, &m_UpVector, &T);
}

//void CameraClass::Render()
//{
//	D3DXVECTOR3 up; // 摄像机的上方
//	D3DXVECTOR3 position; // 摄像机的位置
//	D3DXVECTOR3 lookAt; // 摄像机的目标位置
//	float yaw = 0.0f;
//	float pitch = 0.0f;
//	float roll = 0.0f;
//
//	D3DXMATRIX rotationMatrix;	// 旋转矩阵
//	
//	// 向上的向量
//	up.x = 0.0f;
//	up.y = 1.0f;
//	up.z = 0.0f;
//	
//	// 设置摄像机的位置
//	//position.x = m_positionX;
//	//position.y = m_positionY;
//	//position.z = m_positionZ;
//	
//	// 设置摄像机lookat的方向.
//	//lookAt.x = 0.0f;
//	//lookAt.y = 0.0f;
//	//lookAt.z = 1.0f;
//
//	D3DXVec3Normalize(&lookAt, &position);
//	lookAt = lookAt * (-1);
//
//	// 得到弧度单位的欧拉旋转 yaw (Y axis), pitch (X axis), 以及 roll (Z axis) .
//	//pitch = m_rotationX * 0.0174532925f;
//	//yaw = m_rotationY * 0.0174532925f;
//	//roll = m_rotationZ * 0.0174532925f;
//
//	// 得到旋转矩阵.
//	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
//
//	// 变换lookat和up向量，一般view能够在原点被正确旋转.
//	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
//	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
//
//	// 平移旋转后的摄像机位置, 因为位移之后的摄像机的位置
//	lookAt = position + lookAt;
//
//	// 创建view矩阵.
//	//D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
//}

void CameraClass::GetViewMatrix(D3DXMATRIX& rViewMatrix)
{
	D3DXVec3Normalize(&m_LookAtVetor, &m_LookAtVetor);

	D3DXVec3Cross(&m_UpVector, &m_RightVetor, &m_LookAtVetor);
	D3DXVec3Normalize(&m_UpVector, &m_UpVector);
	
	D3DXVec3Cross(&m_RightVetor, &m_LookAtVetor, &m_UpVector);
	D3DXVec3Normalize(&m_RightVetor, &m_RightVetor);

	//// 生成view矩阵: 
	//float x = -D3DXVec3Dot(&m_RightVetor, &m_PosVector);
	//float y = -D3DXVec3Dot(&m_UpVector, &m_PosVector);
	//float z = -D3DXVec3Dot(&m_LookAtVetor, &m_PosVector);
	D3DXVECTOR3 posLookAt = m_PosVector + m_LookAtVetor;
	// 创建view矩阵.
	//D3DXMatrixLookAtLH(&rViewMatrix, &m_PosVector, &m_PosVector, &m_UpVector);
	D3DXMatrixLookAtLH(&rViewMatrix, &m_PosVector, &posLookAt , &m_UpVector);
	
	return;
}