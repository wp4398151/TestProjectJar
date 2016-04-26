#include "Role.h"
#include "RoleTable.h"
#include <cmath>

CRole::CRole(int timeIntervalms){
	m_Pos = Vec2(RoleTable::m_Row[0].m_StartPosX, RoleTable::m_Row[0].m_StartPosY);
	m_Speed = RoleTable::m_Row[0].m_Speed;
	RoleTable::m_Row[0].m_TextureFile;
	m_intervalFrame = (m_Speed/1000.0) * timeIntervalms;
	m_bMove = false;
}

CRole::~CRole(){

}

wchar_t* CRole::GetTextureFile(){
	return RoleTable::m_Row[0].m_TextureFile;
}

// 在移动时就计算好每一帧需要移动的距离，使得每一帧不用再去根据时间计算,
// 而是直接加帧移动向量
void CRole::Move(Vec2 pos)
{
	m_TargetPos = pos;
	m_TargetVec2Frame = m_TargetPos - m_Pos;
	m_TargetVec2Frame.normalize();
	m_TargetVec2Frame = m_TargetVec2Frame*m_intervalFrame;
	m_bMove = true;
}

void CRole::Frame(int elapse)
{
	if(m_bMove)
	{
		Vec2 dis = m_Pos - m_TargetPos;
		if (dis.length() <= m_intervalFrame)
		{
			m_bMove = false;
			m_Pos = m_TargetPos;
		}
		else
		{
			m_Pos = m_Pos + m_TargetVec2Frame;
		}
	}
}