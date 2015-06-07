#include "stdafx.h"
#include <tchar.h>
#include "DebugConsole.h"
#include "GameLogic.h"
#include "../RenderCore/Direct3D11.h"
#include "../RenderCore/BaseMesh.h"
#include "../RenderCore/DebugMesh.h"
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
	gRenderer->ClearRenderTargetView ( 1.0f, 1.0f, 1.0f, 1.0f );
	gRenderer->Present ();
	//���� ���� ���� ��ó��
}

bool CGameLogic::InitShaders( void )
{
	DebugMsg( "Start Initialize Shaders\n" );

	ON_FAIL_RETURN( gRenderer->CreateVertexShader( _T( "../Shader/vsTutorial.fx" ), "vs_4_0" ) );
	ON_FAIL_RETURN( gRenderer->CreatePixelShader( _T( "../Shader/psTutorial.fx" ), "ps_4_0" ) );

	DebugMsg( "Complete Initialize Shaders\n" );

	return true;
}

bool CGameLogic::InitModel( void )
{
	TriangleMesh mesh;

	mesh.Load( );

	return true;
}

bool CGameLogic::Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight )
{
	ON_FAIL_RETURN( gRenderer->InitializeRenderer( hwnd, wndWidth, wndHeight ) );
	ON_FAIL_RETURN( InitShaders( ) );
	ON_FAIL_RETURN( InitModel( ) );
	gRenderer->PushViewPort( 0, 0, wndWidth, wndHeight );
	
	return true;
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