#include "stdafx.h"
#include "WpDrawObj.h"
#include "Helper.h"
#include <math.h>
#include <sstream>

using namespace std;

BOOL CWpDrawObj::m_bExit = FALSE;
INT CWpDrawObj::m_tickCount = 0 ;

CFreeSpaceGirdStat CFreeSpaceGirdStat::m_sStat;

CObstacleGirdStat CObstacleGirdStat::m_sStat;

CObstacleArranger CObstacleArranger::m_sCObstacleArranger;

CMoveSelector CMoveSelector::m_sCMoveSelector;

// 初始化格子
VOID WpGirdObj::Init(INT x, INT y, INT width)
{
	m_Pt.x = x;
	m_Pt.y = y;
	m_width = width;
	m_pStat = &CFreeSpaceGirdStat::GetInstance();
	Reset();
	return;
}

// 绘制格子
VOID WpGirdObj::Draw(HWND hWnd, HDC hdc)
{
	Rectangle(hdc, m_Pt.x * m_width, m_Pt.y * m_width, m_Pt.x* m_width + m_width, m_Pt.y* m_width + m_width);
	RECT rect = { m_Pt.x * m_width, m_Pt.y * m_width, m_Pt.x* m_width + m_width, m_Pt.y* m_width + m_width };
	m_pStat->Draw(hWnd, hdc, rect);

	//路径
	if (m_bIsTouched == TRUE)
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(1, 128, 1));
		FillRect(hdc, &rect, hBrush);
		DeleteObject(hBrush);
	}

	return;
}

// 绘制世界地图
VOID CWpDrawObj::Draw(HDC hdc)
{
	for (int i = 0; i < MAX_X ; i++ )
	{
		for (int j = 0; j < MAX_Y ; j++)
		{
			m_WpGirdObjs[i][j].Draw(m_Hwnd, hdc);
		}
	}
	m_Robot.Draw(m_Hwnd, hdc);
	return;
}

 CWpDrawObj::CWpDrawObj()
{

}

 VOID CWpDrawObj::Reset()
 {
	for (int i = 0; i < MAX_X; i++)
	{
		for (int j = 0; j < MAX_Y; j++)
		{
			m_WpGirdObjs[i][j].Reset();
		}
	}
	return;
 }

VOID CWpDrawObj::Init(HWND hwnd)
{
	m_Hwnd = hwnd;
	m_pOpInst = &CObstacleArranger::GetInstance();

	for (int i = 0; i < MAX_X; i++)
	{
		for (int j = 0; j < MAX_Y; j++)
		{
			m_WpGirdObjs[i][j].Init(i, j, GIRDWIDTH);
		}
	}
	return;
}

 // 控制设置障碍的状态
VOID CObstacleArranger::Do(CWpDrawObj& rDrawObj, POINT &rPt)
{
	rDrawObj.SetGrid(rPt.x / GIRDWIDTH, rPt.y / GIRDWIDTH, CObstacleGirdStat::GetInstance());
}

// 控制行走的状态
VOID CMoveSelector::Do(CWpDrawObj& rDrawObj, POINT &rPt)
{
	//rDrawObj.Goto(rPt);							// 直接移动
	//rDrawObj.m_Robot.StartWalk(rDrawObj, rPt);	// 阻挡移动

	//vector<WpGirdObj*> nodesTouched;			// touched node
	//FPOINT fpt;
	//FPOINT fptCurPos;
	//fptCurPos.x = rDrawObj.m_Robot.m_Pos.x / GIRDWIDTH;
	//fptCurPos.y = rDrawObj.m_Robot.m_Pos.y / GIRDWIDTH;
	//fpt.x = (float)(rPt.x / (1.0 * GIRDWIDTH));
	//fpt.y = (float)(rPt.y / (1.0 * GIRDWIDTH));
	//rDrawObj.s_pPathFinder->FindPath(rDrawObj, fptCurPos, fpt, nodesTouched);
}

// 处理点击
VOID CWpDrawObj::Do(POINT &rPt)
{
	m_pOpInst->Do(*this, rPt);
	InvalidateRect(m_Hwnd, NULL, FALSE);
}

WpRobot::WpRobot()
{
	m_Pos.x = 2;
	m_Pos.y = 2;
	m_Tarpos.x = 2;
	m_Tarpos.x = 2;
}

// 绘制机器人本身
VOID WpRobot::Draw(HWND hwnd, HDC hdc)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 128, 0));
	HGDIOBJ oldobj = SelectObject(hdc, hBrush);
	Ellipse(hdc, (INT)m_Pos.x - ROBOTRADIUS, (INT)m_Pos.y - ROBOTRADIUS,
					(INT)m_Pos.x + ROBOTRADIUS, (INT)m_Pos.y + ROBOTRADIUS);
	SelectObject(hdc, oldobj);
	DeleteObject(hBrush);
}

VOID WpRobot::OnHeartBeat(CWpDrawObj& rDrawObj, INT elapse)
{

	if (m_Pos.x != m_Tarpos.x && m_Pos.y != m_Tarpos.y)
	{
		// 如果已经在目标点,点就不需要移动
		FPOINT fCurPos(m_Pos);
		FPOINT fEndPos = fCurPos + ( m_Speeddir * (float)elapse );
		
		if (Get2PointScale(fEndPos, m_Pos) > Get2PointScale(m_Tarpos, m_Pos) )
		{
			m_Pos = m_Tarpos;
		}
		else
		{
			m_Pos = fEndPos ;
		}
	}
}

BOOL WpRobot::CalCollisionPos(CWpDrawObj& rObj,
	FPOINT &rFpt,		// 实际目的地点
	FPOINT &rStart,		// 所在地
	FPOINT &rEnd,		// 目的地
	float fRateXY,		// 路径比
	FPOINT &rFptDir		// 向量	
	)
{
	// 先计算第一个遇到的方块
	FPOINT ptCollisionPointX = { 0, 0 };
	FPOINT ptCollisionPointY = { 0, 0 };
	int iEndX = (int)floor(rEnd.x);
	int iEndY = (int)floor(rEnd.y);
	int iStarX = (int)floor(rStart.x);
	int iStarY = (int)floor(rStart.y);

	if (iStarX == iEndX && iStarY == iEndY)
	{
		rFpt = rEnd;
		return true;
	}
	float fEndDistVal = abs(rEnd.x - rStart.x) + abs(rEnd.y - rStart.y);
	
	int tempGirdx = 0;
	int tempGirdy = 0;
	// 撞到竖线
	bool xCol = false;
	ptCollisionPointX.x = RoundDir(rStart.x, rFptDir.x);
	ptCollisionPointX.y = ((ptCollisionPointX.x - rStart.x) / fRateXY) + rStart.y;

	while (fEndDistVal > (abs(ptCollisionPointX.x - rStart.x) + abs(ptCollisionPointX.y - rStart.y)))
	{
		if (rFptDir.x > 0)
		{
			// 如果大于0 则不需要移动x
			tempGirdx = (INT)floor(ptCollisionPointX.x);
		}
		else
		{
			// 小于0 需要移动到下一个整数
			tempGirdx = (INT)floor(StepDir(ptCollisionPointX.x, rFptDir.x));
		}
		tempGirdy = (INT)floor(ptCollisionPointX.y);
		
		if (FALSE == rObj.CanWalkDirect(tempGirdx, tempGirdy))
		{
			xCol = true;
			break;
		}
		ptCollisionPointX.x = StepDir(ptCollisionPointX.x, rFptDir.x);
		ptCollisionPointX.y = ((ptCollisionPointX.x - rStart.x) / fRateXY) + rStart.y;
	} 
	//--------------------------------------------------------

	// 遇到横线
	bool yCol = false;
	ptCollisionPointY.y = RoundDir(rStart.y, rFptDir.y);
	ptCollisionPointY.x =  ((ptCollisionPointY.y - rStart.y) * fRateXY) + rStart.x;
	
	while (fEndDistVal > (abs(ptCollisionPointY.x - rStart.x) + abs(ptCollisionPointY.y - rStart.y)))
	{
		if (rFptDir.y > 0)
		{
			// 如果大于0 则不需要移动x
			tempGirdy = (INT)floor(ptCollisionPointY.y);
		}
		else
		{
			tempGirdy = (INT)floor(StepDir(ptCollisionPointY.y, rFptDir.y));
			// 小于0 需要移动到下一个整数
		}
		tempGirdx = (INT)floor(ptCollisionPointY.x);
		
		if (FALSE == rObj.CanWalkDirect(tempGirdx, tempGirdy))
		{
			yCol = true;
			break;
		}
		ptCollisionPointY.y = StepDir(ptCollisionPointY.y, rFptDir.y);
		ptCollisionPointY.x =  ((ptCollisionPointY.y - rStart.y) * fRateXY) + rStart.x;
	}
	//--------------------------------------------------------
	
	// 比较是先遇到那一条线
	if (yCol && xCol)
	{
		if ((abs(ptCollisionPointX.x - rStart.x) + abs(ptCollisionPointX.y -rStart.y )) >
			(abs(ptCollisionPointY.x - rStart.x) + abs(ptCollisionPointY.y - rStart.y )))
		{
			//如果遇到了障碍物就退回到边界
			
			rFpt = ptCollisionPointY - (rFptDir * (ROBOTRADIUS / (float)GIRDWIDTH));
		}
		else
		{
			rFpt = ptCollisionPointX - (rFptDir * (ROBOTRADIUS / (float)GIRDWIDTH)) ;
		}
	}
	else if (yCol)
	{
			rFpt = ptCollisionPointY - (rFptDir * (ROBOTRADIUS / (float)GIRDWIDTH)) ;
	}
	else if (xCol)
	{
			rFpt = ptCollisionPointX - (rFptDir *( ROBOTRADIUS / (float)GIRDWIDTH)) ;
	}
	else
	{
		// 如果没有碰撞，直接移动到End;
		rFpt = rEnd;
	}
	return TRUE;
}

VOID WpRobot::StartWalk(CWpDrawObj& rObj, POINT &rPt)
{
	if (FALSE == rObj.CanWalk((float)rPt.x, (float)rPt.y))
	{
		return;
	}
	// 简单的情况，直接过去
	// m_Pos.x = rPt.x;
	// m_Pos.y = rPt.y;

	// 需要检测是否可以通过
	// 0 转换成float 
	FPOINT fptCur;
	FPOINT fptTar;
	FPOINT fptDir;
	FPOINT rFptStop;
	fptCur.x = m_Pos.x / (float)GIRDWIDTH;
	fptCur.y = m_Pos.y / (float)GIRDWIDTH;
	fptTar.x = rPt.x / (float)GIRDWIDTH;
	fptTar.y = rPt.y / (float)GIRDWIDTH;
	fptDir.x = fptTar.x - fptCur.x;
	fptDir.y = fptTar.y - fptCur.y;

	// 1 算出移动向量
	float rateXY = fptDir.x / fptDir.y;

	Nomalize(fptDir);

	CalCollisionPos(rObj, rFptStop, fptCur,
		fptTar, rateXY, fptDir);

	rPt.x = (INT)round(rFptStop.x * GIRDWIDTH);
	rPt.y = (INT)round(rFptStop.y * GIRDWIDTH);
	m_Tarpos = rPt;
	
	FPOINT pt;
	pt.x = m_Tarpos.x - (float)m_Pos.x;
	pt.y = m_Tarpos.y - (float)m_Pos.y;
	Nomalize(pt);
	m_Speeddir = pt*(ROBOTSPEED/(float)1000.0);
	return;
}

// 移动坐标
VOID WpRobot::GoTo(CWpDrawObj& rObj, POINT &rPt)
{
	if (FALSE == rObj.CanWalk((float)rPt.x, (float)rPt.y))
	{
		return;
	}
	// 简单的情况，直接过去
	// m_Pos.x = rPt.x;
	// m_Pos.y = rPt.y;

	// 需要检测是否可以通过
	// 0 转换成float 
	FPOINT fptCur;
	FPOINT fptTar;
	FPOINT fptDir;
	FPOINT rFptStop;
	fptCur.x = m_Pos.x / (float)GIRDWIDTH;
	fptCur.y = m_Pos.y / (float)GIRDWIDTH;
	fptTar.x = rPt.x / (float)GIRDWIDTH;
	fptTar.y = rPt.y / (float)GIRDWIDTH;
	fptDir.x = fptTar.x - fptCur.x;
	fptDir.y = fptTar.y - fptCur.y;

	// 1 算出移动向量
	float rateXY = fptDir.x / fptDir.y;

	Nomalize(fptDir);

	CalCollisionPos(rObj, rFptStop, fptCur,
						fptTar, rateXY, fptDir);
	
	m_Pos.x = round(rFptStop.x * GIRDWIDTH);
	m_Pos.y = round(rFptStop.y * GIRDWIDTH);
}

BOOL CWpDrawObj::CanWalkDirect(INT x, INT y)
{
	return m_WpGirdObjs[x][y].CanWalk();
}

// 是否能够行走
BOOL CWpDrawObj::CanWalk(float x, float y)
{
	x = x / GIRDWIDTH;
	y = y / GIRDWIDTH;
	return m_WpGirdObjs[(int)round(x)][(int)round(y)].CanWalk();
}

// 心跳函数，用来心跳逻辑
VOID CWpDrawObj::Start(CWpDrawObj* rDrawObj)
{
	INT tickCount = GetTickCount();
	INT timeElapse = tickCount - m_tickCount;
	while(true)
	{
		if (m_bExit)
		{
			break;
		}

		Sleep(25);
		tickCount = GetTickCount();
		timeElapse = tickCount - m_tickCount;
		rDrawObj->m_Robot.OnHeartBeat(*rDrawObj, timeElapse);
		m_tickCount = tickCount;
		
		InvalidateRect(rDrawObj->m_Hwnd, NULL, FALSE);
	}
	return ;
}

VOID CWpDrawObj::CleartTouchData()
{

	for (int i = 0; i < MAX_X; i++)
	{
		for (int j = 0; j < MAX_Y; j++)
		{
			m_WpGirdObjs[i][j].Reset();

		}
	}
	return;
}


VOID CWpDrawObj::GetSize(UINT &rx, UINT &ry)
{
	rx = MAX_X;
	ry = MAX_Y;
}