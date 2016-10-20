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

// ��ʼ������
VOID WpGirdObj::Init(INT x, INT y, INT width)
{
	m_Pt.x = x;
	m_Pt.y = y;
	m_width = width;
	m_pStat = &CFreeSpaceGirdStat::GetInstance();
	Reset();
	return;
}

// ���Ƹ���
VOID WpGirdObj::Draw(HWND hWnd, HDC hdc)
{
	Rectangle(hdc, m_Pt.x * m_width, m_Pt.y * m_width, m_Pt.x* m_width + m_width, m_Pt.y* m_width + m_width);
	RECT rect = { m_Pt.x * m_width, m_Pt.y * m_width, m_Pt.x* m_width + m_width, m_Pt.y* m_width + m_width };
	m_pStat->Draw(hWnd, hdc, rect);

	//·��
	if (m_bIsTouched == TRUE)
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(1, 128, 1));
		FillRect(hdc, &rect, hBrush);
		DeleteObject(hBrush);
	}

	return;
}

// ���������ͼ
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

 // ���������ϰ���״̬
VOID CObstacleArranger::Do(CWpDrawObj& rDrawObj, POINT &rPt)
{
	rDrawObj.SetGrid(rPt.x / GIRDWIDTH, rPt.y / GIRDWIDTH, CObstacleGirdStat::GetInstance());
}

// �������ߵ�״̬
VOID CMoveSelector::Do(CWpDrawObj& rDrawObj, POINT &rPt)
{
	//rDrawObj.Goto(rPt);							// ֱ���ƶ�
	//rDrawObj.m_Robot.StartWalk(rDrawObj, rPt);	// �赲�ƶ�

	//vector<WpGirdObj*> nodesTouched;			// touched node
	//FPOINT fpt;
	//FPOINT fptCurPos;
	//fptCurPos.x = rDrawObj.m_Robot.m_Pos.x / GIRDWIDTH;
	//fptCurPos.y = rDrawObj.m_Robot.m_Pos.y / GIRDWIDTH;
	//fpt.x = (float)(rPt.x / (1.0 * GIRDWIDTH));
	//fpt.y = (float)(rPt.y / (1.0 * GIRDWIDTH));
	//rDrawObj.s_pPathFinder->FindPath(rDrawObj, fptCurPos, fpt, nodesTouched);
}

// ������
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

// ���ƻ����˱���
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
		// ����Ѿ���Ŀ���,��Ͳ���Ҫ�ƶ�
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
	FPOINT &rFpt,		// ʵ��Ŀ�ĵص�
	FPOINT &rStart,		// ���ڵ�
	FPOINT &rEnd,		// Ŀ�ĵ�
	float fRateXY,		// ·����
	FPOINT &rFptDir		// ����	
	)
{
	// �ȼ����һ�������ķ���
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
	// ײ������
	bool xCol = false;
	ptCollisionPointX.x = RoundDir(rStart.x, rFptDir.x);
	ptCollisionPointX.y = ((ptCollisionPointX.x - rStart.x) / fRateXY) + rStart.y;

	while (fEndDistVal > (abs(ptCollisionPointX.x - rStart.x) + abs(ptCollisionPointX.y - rStart.y)))
	{
		if (rFptDir.x > 0)
		{
			// �������0 ����Ҫ�ƶ�x
			tempGirdx = (INT)floor(ptCollisionPointX.x);
		}
		else
		{
			// С��0 ��Ҫ�ƶ�����һ������
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

	// ��������
	bool yCol = false;
	ptCollisionPointY.y = RoundDir(rStart.y, rFptDir.y);
	ptCollisionPointY.x =  ((ptCollisionPointY.y - rStart.y) * fRateXY) + rStart.x;
	
	while (fEndDistVal > (abs(ptCollisionPointY.x - rStart.x) + abs(ptCollisionPointY.y - rStart.y)))
	{
		if (rFptDir.y > 0)
		{
			// �������0 ����Ҫ�ƶ�x
			tempGirdy = (INT)floor(ptCollisionPointY.y);
		}
		else
		{
			tempGirdy = (INT)floor(StepDir(ptCollisionPointY.y, rFptDir.y));
			// С��0 ��Ҫ�ƶ�����һ������
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
	
	// �Ƚ�����������һ����
	if (yCol && xCol)
	{
		if ((abs(ptCollisionPointX.x - rStart.x) + abs(ptCollisionPointX.y -rStart.y )) >
			(abs(ptCollisionPointY.x - rStart.x) + abs(ptCollisionPointY.y - rStart.y )))
		{
			//����������ϰ�����˻ص��߽�
			
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
		// ���û����ײ��ֱ���ƶ���End;
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
	// �򵥵������ֱ�ӹ�ȥ
	// m_Pos.x = rPt.x;
	// m_Pos.y = rPt.y;

	// ��Ҫ����Ƿ����ͨ��
	// 0 ת����float 
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

	// 1 ����ƶ�����
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

// �ƶ�����
VOID WpRobot::GoTo(CWpDrawObj& rObj, POINT &rPt)
{
	if (FALSE == rObj.CanWalk((float)rPt.x, (float)rPt.y))
	{
		return;
	}
	// �򵥵������ֱ�ӹ�ȥ
	// m_Pos.x = rPt.x;
	// m_Pos.y = rPt.y;

	// ��Ҫ����Ƿ����ͨ��
	// 0 ת����float 
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

	// 1 ����ƶ�����
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

// �Ƿ��ܹ�����
BOOL CWpDrawObj::CanWalk(float x, float y)
{
	x = x / GIRDWIDTH;
	y = y / GIRDWIDTH;
	return m_WpGirdObjs[(int)round(x)][(int)round(y)].CanWalk();
}

// �������������������߼�
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