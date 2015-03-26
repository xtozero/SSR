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
	//게임 로직 수행 전처리
}

void CGameLogic::ProcessLogic ( void )
{
	//게임 로직 수행
}

void CGameLogic::EndLogic ( void )
{
	//게임 로직 수행 후처리
}

void CGameLogic::UpdateLogic ( void )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	CTimer::GetInstance ().Tick ();

	StartLogic ();
	ProcessLogic ();
	EndLogic ();
}