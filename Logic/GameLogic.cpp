#include "stdafx.h"
#include "DebugConsole.h"
#include "GameLogic.h"
#include "Timer.h"


CGameLogic::CGameLogic ( )
{
	ShowDebugConsole ();
}


CGameLogic::~CGameLogic ()
{

}

void CGameLogic::StartLogic ( void )
{
	//���� ���� ���� ��ó��
}

void CGameLogic::ProcessLogic ( void )
{
	//���� ���� ����
}

void CGameLogic::EndLogic ( void )
{
	//���� ���� ���� ��ó��
}

void CGameLogic::UpdateLogic ( void )
{
	// �� �������� ���� ElapsedTime ����
	CTimer::GetInstance ().Tick ();

	StartLogic ();
	ProcessLogic ();
	EndLogic ();
}