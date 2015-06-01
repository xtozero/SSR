#include "stdafx.h"
#include "DebugConsole.h"
#include "GameLogic.h"
#include "../RenderCore/Direct3D11.h"
#include "../RenderCore/IRenderer.h"
#include "Timer.h"

IRenderer* gRenderer = CreateDirect3D11Renderer ();

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
	gRenderer->ClearRenderTargetView ( 1.0f, 0.0f, 0.0f, 1.0f );
	gRenderer->Present ();
	//게임 로직 수행 후처리
}

bool CGameLogic::Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight )
{
	return gRenderer->InitializeRenderer ( hwnd, wndWidth, wndHeight );
}

void CGameLogic::UpdateLogic ( void )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	CTimer::GetInstance ().Tick ();

	StartLogic ();
	ProcessLogic ();
	EndLogic ();
}

CGameLogic::CGameLogic ( )
{
	ShowDebugConsole ( );
}


CGameLogic::~CGameLogic ( )
{

}