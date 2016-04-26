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
	Vec2 m_Pos;				// 当前位置
	Vec2 m_TargetPos;		// 目标位置
	float m_Speed;			// 当前速度
	float m_intervalFrame;	// 每帧的移动间隔
	Vec2 m_TargetVec2Frame;	// 每一帧的移动向量
	bool m_bMove;
};