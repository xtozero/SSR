#include "stdafx.h"
#include <tchar.h>
#include "DebugConsole.h"
#include "GameLogic.h"
#include "GameObject.h"
#include "../Shared/Util.h"
#include "../RenderCore/Direct3D11.h"
#include "../RenderCore/BaseMesh.h"
#include "../RenderCore/DebugMesh.h"
#include "Timer.h"

IRenderer* gRenderer = CreateDirect3D11Renderer ();

void CGameLogic::StartLogic ( void )
{
	//게임 로직 수행 전처리
}

void CGameLogic::ProcessLogic ( void )
{
	//게임 로직 수행
	FOR_EACH_VEC( m_gameObjects, iter )
	{
		( *iter )->Think( );
	}
}

void CGameLogic::EndLogic ( void )
{
	//게임 로직 수행 후처리
	m_mainCamera.UpdateToRenderer( gRenderer );
	SceneBegin( );
	DrawScene( );
	SceneEnd( );
}

bool CGameLogic::InitShaders( void )
{
	DebugMsg( "Start Initialize Shaders\n" );

	ON_FAIL_RETURN( gRenderer->CreateVertexShader( _T( "../Shader/vsTutorial.fx" ), "vs_4_0" ) );
	ON_FAIL_RETURN( gRenderer->CreatePixelShader( _T( "../Shader/psTutorial.fx" ), "ps_4_0" ) );

	DebugMsg( "Complete Initialize Shaders\n" );

	return true;
}

bool CGameLogic::LoadScene( void )
{
	auto keyValue = m_sceneLoader.LoadSceneFromFile( _T( "../Script/TestScene.txt" ), m_gameObjects );

	if ( !keyValue )
	{
		return false;
	}

	return true;
}

void CGameLogic::SceneBegin( void )
{
	gRenderer->SceneBegin( );
}

void CGameLogic::DrawScene( void )
{
	FOR_EACH_VEC( m_gameObjects, object )
	{
		UpdateWorldMatrix( object->get( ) );
		( *object )->Render( );
	}
}

void CGameLogic::SceneEnd( void )
{
	gRenderer->SceneEnd( );
}

void CGameLogic::UpdateWorldMatrix( const CGameObject* object )
{
	if ( object )
	{
		gRenderer->UpdateWorldMatrix( object->GetTransformMatrix( ) );
	}
}

bool CGameLogic::Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight )
{
	ON_FAIL_RETURN( gRenderer->InitializeRenderer( hwnd, wndWidth, wndHeight ) );
	ON_FAIL_RETURN( InitShaders( ) );
	ON_FAIL_RETURN( gRenderer->InitMaterial( ) );
	ON_FAIL_RETURN( LoadScene( ) );
	gRenderer->PushViewPort( 0.0f, 0.0f, static_cast<float>( wndWidth ), static_cast<float>( wndHeight ) );
	
	IRenderView* view = gRenderer->GetCurrentRenderView( );
	
	if ( view )
	{
		view->CreatePerspectiveFovLHMatrix( D3DXToRadian( 60 ),
											static_cast<float>( wndWidth ) / wndHeight,
											1.f,
											1500.f );
	}
	else
	{
		return false;
	}

	return true;
}

void CGameLogic::UpdateLogic ( void )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	CTimer::GetInstance ().Tick ();

	StartLogic ();
	ProcessLogic ();
	EndLogic ();
}

bool CGameLogic::HandleWindowMessage( const MSG& msg )
{
	switch ( msg.message )
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		HandleWIndowKeyInput( msg.message, msg.wParam, msg.lParam );
		return true;
		break;
	default:
		//Message UnHandled;
		return false;
		break;
	}
}

void CGameLogic::HandleWIndowKeyInput( const int message, const WPARAM wParam, const LPARAM lParam )
{
	switch ( message )
	{
	case WM_KEYDOWN:
		{
					 TCHAR keyName[256] = { 0, };
					 GetKeyNameText( lParam, keyName, 256 );
					 DebugMsg( "%s\n", keyName );
					 switch ( wParam )
					 {
					 case VK_UP:
						 m_mainCamera.Move( 0.f, 0.f, 10.f );
						 break;
					 case VK_DOWN:
						 m_mainCamera.Move( 0.f, 0.f, -10.f );
						 break;
					 case VK_RIGHT:
						 m_mainCamera.Move( 10.f, 0.f, 0.f );
						 break;
					 case VK_LEFT:
						 m_mainCamera.Move( -10.f, 0.f, 0.f );
						 break;
					 default:
						 break;
					 }
		}
	default:
		break;
	}
}

CGameLogic::CGameLogic( )
{
	ShowDebugConsole( );
}

CGameLogic::~CGameLogic ( )
{
}