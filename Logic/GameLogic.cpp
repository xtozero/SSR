#include "stdafx.h"
#include "DebugConsole.h"
#include "GameLogic.h"
#include "../RenderCore/Direct3D11.h"
#include "../RenderCore/IRenderer.h"
#include "Timer.h"

IRenderer* gRenderer = CreateDirect3D11Renderer ();

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
	gRenderer->ClearRenderTargetView ( 1.0f, 0.0f, 0.0f, 1.0f );
	gRenderer->Present ();
	//���� ���� ���� ��ó��
}

bool CGameLogic::Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight )
{
	return gRenderer->InitializeRenderer ( hwnd, wndWidth, wndHeight );
}

void CGameLogic::UpdateLogic ( void )
{
	// �� �������� ���� ElapsedTime ����
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