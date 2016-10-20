#ifndef _WPDRAOBJ_H_
#define _WPDRAOBJ_H_
#include "stdafx.h"
#include "Helper.h"
#include <thread>
#define MAX_X 50 
#define MAX_Y 35 
#define GIRDWIDTH 20
#define ROBOTRADIUS 10
#define ROBOTSPEED 100

// 帧数间隔,毫秒
#define FRAMETIMEINTERVAL 25 
class PathFinder;

enum EGirdObjType
{
	eFreeSpaceGird,
	eObstacleGird,
};

class IGirdStat
{
public:
	virtual EGirdObjType GetType() = 0;
	virtual BOOL CanWalk() = 0;
	virtual VOID Draw(HWND hwnd, HDC hdc, RECT &rRect) = 0;
};

class CFreeSpaceGirdStat : public IGirdStat
{
public:	
	virtual EGirdObjType GetType(){ return eFreeSpaceGird; }
	virtual BOOL CanWalk(){ return TRUE; }
	static CFreeSpaceGirdStat& GetInstance(){ return m_sStat; }
	virtual VOID Draw(HWND hwnd, HDC hdc, RECT &rRect){ }
private:
	CFreeSpaceGirdStat(){}
	static CFreeSpaceGirdStat m_sStat;
};

class CObstacleGirdStat : public IGirdStat
{
public:
	virtual EGirdObjType GetType(){ return eObstacleGird; }
	virtual BOOL CanWalk(){ return FALSE; }
	static CObstacleGirdStat & GetInstance(){ return m_sStat; }
	virtual VOID Draw(HWND hwnd, HDC hdc, RECT &rRect){
		HBRUSH hBrush = CreateSolidBrush(RGB(128, 128, 128));
		FillRect(hdc, &rRect, hBrush);
		DeleteObject(hBrush);
	}
private:
	static CObstacleGirdStat m_sStat;
	CObstacleGirdStat(){}
};

class WpGirdObj
{
public:
	WpGirdObj(){
		m_Pt.x = 0;
		m_Pt.y = 0;
		m_width = 0;
		m_pStat = NULL;
		m_StepCount = 0;
	}
	virtual VOID Init(INT x, INT y, INT width);
	virtual VOID SetState(IGirdStat& rStat){ m_pStat = &rStat; }
	virtual BOOL CanWalk(){ if (m_pStat) return m_pStat->CanWalk(); return FALSE; }
	virtual VOID Draw(HWND hWnd, HDC hdc);

	virtual VOID Reset(){
		m_bIsTouched = FALSE;
		m_ParentPt.x = 0;
		m_ParentPt.y = 0;
		m_priority = 0;
		m_StepCount = 0;
		m_Order = 0;
	}

	virtual VOID SetPriorityData(POINT &rPT, INT rPriority)
	{
		m_ParentPt = rPT;
		m_priority = rPriority;
		return;
	}

	virtual VOID SetTouchstatu(BOOL isTouched)
	{
		m_bIsTouched = isTouched;
	}

	virtual BOOL GetTouchedPt(POINT &rPT, INT &rPriority){
		if (m_bIsTouched)
		{
			rPT = m_ParentPt;
			rPriority = m_priority;
		}
		return m_bIsTouched;
	}
public:
	POINT m_Pt;
	INT m_width;
	IGirdStat *m_pStat;

	POINT m_ParentPt;
	INT m_priority;
	INT m_StepCount;
	BOOL m_bIsTouched;
	INT m_Order;
};

class CWpDrawObj;

class COpraterObj
{
public:
	virtual VOID Do(CWpDrawObj& rDrawObj, POINT &rPt) = 0;
};

class CObstacleArranger : public COpraterObj
{
public:
	virtual VOID Do(CWpDrawObj& rDrawObj, POINT &rPt);

	static CObstacleArranger& GetInstance(){ return m_sCObstacleArranger; }
private:
	static CObstacleArranger m_sCObstacleArranger;
	CObstacleArranger(){}
};

class CMoveSelector : public COpraterObj
{
public:
	virtual VOID Do(CWpDrawObj& rDrawObj, POINT &rPt);
	static CMoveSelector& GetInstance(){ return m_sCMoveSelector; }
private:
	static CMoveSelector m_sCMoveSelector;
	CMoveSelector(){ }
};

class WpRobot
{
public:	
	WpRobot();
	virtual VOID StartWalk(CWpDrawObj& rObj, POINT &rPt);
	virtual VOID Draw(HWND hwnd, HDC hdc);
	virtual VOID GoTo(CWpDrawObj& rObj, POINT &rPt);
	BOOL CalCollisionPos(CWpDrawObj& rObj, FPOINT &rFpt, FPOINT &rStart,
							FPOINT &rEnd, float fRateXY, FPOINT &rFptDir);

	virtual VOID OnHeartBeat(CWpDrawObj& rDrawObj, INT elapse);

	FPOINT m_Pos;
private:
	FPOINT m_Speeddir;		// 速度向量
	POINT m_Tarpos;			// 目标
	int m_speed;			// 速度
};

class ICheckWalkble
{
public:
	virtual VOID Reset() = 0;
	virtual BOOL CanWalkDirect(INT x, INT y) = 0;
	virtual VOID GetSize(UINT &x, UINT&y) = 0;
	virtual VOID GetPosisionPT(float x, float y, POINT &rPt) = 0;
	virtual WpGirdObj* GetGird(INT x, INT y) = 0;
};

class CWpDrawObj : public ICheckWalkble
{
public:
	CWpDrawObj();
	VOID Init(HWND hWnd);
	VOID Draw(HDC hdc);
	VOID SetOp(COpraterObj &rOp){ m_pOpInst = &rOp; }
	VOID Reset();
	VOID SetGrid(INT x, INT y, IGirdStat &rStat){
		m_WpGirdObjs[x][y].SetState(rStat);
	}
	WpGirdObj* GetGird(INT x, INT y){
		if (ISINRANGE(x, 0, MAX_X - 1) &&
			ISINRANGE(y, 0, MAX_Y - 1))
		{
			return &m_WpGirdObjs[x][y];
		}
		return NULL;
	}

	VOID GetPosisionPT(float x, float y, POINT &rPt)
	{
		rPt.x = (int)floor(x);
		rPt.y = (int)floor(y);
	}
	VOID Do(POINT &rPt);
	VOID Goto(POINT &rPt){ 
		m_Robot.GoTo(*this, rPt); 
	}

	BOOL CanWalkDirect(INT x, INT y);
	BOOL CanWalk(float x, float y);
	virtual VOID GetSize(UINT &x, UINT&y);
	static VOID Start(CWpDrawObj* rDrawObj);
	
	VOID CleartTouchData();
	WpRobot m_Robot;
	static BOOL m_bExit;

	std::thread *pth;

	static PathFinder *s_pPathFinder;
private:
	WpGirdObj m_WpGirdObjs[MAX_X][MAX_Y];
	COpraterObj *m_pOpInst;
	HWND m_Hwnd;
	static INT m_tickCount;
};

#endif //_WPDRAOBJ_H_