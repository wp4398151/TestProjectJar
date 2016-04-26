////////////////////////////////////////////////////
// role class, specific the leading actor in current version.
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#pragma once

#include "MathLite.h"

class CRole
{
public:
	CRole(int interval);
	~CRole();

	void getPos(Vec2& pos){
		pos = m_Pos;
	}
	void setPos(Vec2 &pos){
		m_Pos = pos;
	}
	wchar_t* GetTextureFile();
	void Move(Vec2 pos);
	void Frame(int elapse);

private:
	Vec2 m_Pos;				// ��ǰλ��
	Vec2 m_TargetPos;		// Ŀ��λ��
	float m_Speed;			// ��ǰ�ٶ�
	float m_intervalFrame;	// ÿ֡���ƶ����
	Vec2 m_TargetVec2Frame;	// ÿһ֡���ƶ�����
	bool m_bMove;
};