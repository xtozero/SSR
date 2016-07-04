#include "stdafx.h"
#include "CameraManager.h"
#include "DebugConsole.h"
#include "GameLogic.h"
#include "GameObject.h"
#include "Timer.h"
#include "UtilWindowInfo.h"

#include "../Engine/ConVar.h"
#include "../Engine/KeyValueReader.h"
#include "../Engine/ConCommand.h"
#include "../RenderCore/BaseMesh.h"
#include "../RenderCore/DebugMesh.h"
#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCOre/IRenderView.h"
#include "../RenderCore/RenderCoreDllFunc.h"
#include "../Shared/Util.h"

#include <ctime>
#include <tchar.h>

namespace
{
	ConVar( showFps, "1", "Show Fps" );
}

IRenderer* gRenderer = CreateDirect3D11Renderer ();
IMeshBuilder* g_meshBuilder = GetMeshBuilder( );

void CGameLogic::StartLogic ( void )
{
	//게임 로직 수행 전처리
}

void CGameLogic::ProcessLogic ( void )
{
	//게임 로직 수행
	FOR_EACH_VEC( g_gameObjects, iter )
	{
		( *iter )->Think( );
	}
}

void CGameLogic::EndLogic ( void )
{
	//그림자 맵 렌더링
	m_shadowManager.Process( m_lightManager, *gRenderer, g_gameObjects );

	//게임 로직 수행 후처리
	m_mainCamera.UpdateToRenderer( gRenderer );
	m_lightManager.UpdateToRenderer( m_mainCamera );
	SceneBegin( );
	DrawScene( );
	SceneEnd( );
}

bool CGameLogic::LoadScene( void )
{
	g_gameObjects.erase( g_gameObjects.begin( ), g_gameObjects.end( ) );
	auto keyValue = m_sceneLoader.LoadSceneFromFile( _T( "../Script/TestScene.txt" ), g_gameObjects );

	if ( !keyValue )
	{
		return false;
	}

	InitCameraProperty( keyValue );

	return true;
}

void CGameLogic::SceneBegin( void ) const
{
	float wndWidth = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetWidth( ) );
	float wndHeight = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetHeight( ) );
	gRenderer->PushViewPort( 0.f, 0.f, wndWidth, wndHeight );

	gRenderer->SceneBegin( );
}

void CGameLogic::DrawScene( void ) const
{
	FOR_EACH_VEC( g_gameObjects, object )
	{
		if ( object->get( )->ShouldDraw( ) )
		{
			UpdateWorldMatrix( object->get( ) );
			(*object)->Render( );
		}
	}
}

void CGameLogic::SceneEnd( void ) const
{
	gRenderer->SceneEnd( );
}

void CGameLogic::UpdateWorldMatrix( CGameObject* object ) const
{
	if ( object )
	{
		gRenderer->UpdateWorldMatrix( object->GetTransformMatrix( ), object->GetInvTransformMatrix( ) );
	}
}

void CGameLogic::InitCameraProperty( std::shared_ptr<KeyValueGroup> keyValue )
{
	CKeyValueIterator found = keyValue->FindKeyValue( _T( "Camera Pos" ) );

	if ( found != nullptr )
	{
		std::vector<String> param;
		UTIL::Split( found->GetString( ), param, ' ' );

		if ( param.size( ) == 3 )
		{
			m_mainCamera.SetOrigin( D3DXVECTOR3( 
				static_cast<float>( _ttof( param[0].c_str( ) ) ),
				static_cast<float>( _ttof( param[1].c_str( ) ) ),
				static_cast<float>( _ttof( param[2].c_str( ) ) ) ) );
		}
	}
}

bool CGameLogic::Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight )
{
	m_wndHwnd = hwnd;
	srand( static_cast<UINT>( time( NULL ) ) );
	CUtilWindowInfo::GetInstance( ).SetRect( wndWidth, wndHeight );

	ON_FAIL_RETURN( gRenderer->InitializeRenderer( hwnd, wndWidth, wndHeight ) );
	
	m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>( wndWidth ), static_cast<float>( wndHeight ) );

	IRenderView* view = gRenderer->GetCurrentRenderView( );
	
	if ( view )
	{
		view->CreatePerspectiveFovLHMatrix( D3DXToRadian( 60 ),
											static_cast<float>( wndWidth ) / wndHeight,
											1.f,
											1500.f );
		m_pickingManager.PushInvProjection( D3DXToRadian( 60 ),
											static_cast<float>( wndWidth ) / wndHeight,
											1.f,
											1500.f );
	}
	else
	{
		return false;
	}

	m_pickingManager.PushCamera( &m_mainCamera );
	m_mouseController.AddListener( &m_pickingManager );
	m_mouseController.AddListener( &m_mainCamera );

	CCameraManager::GetInstance( )->SetCurrentCamera( &m_mainCamera );

	ON_FAIL_RETURN( LoadScene( ) );
	ON_FAIL_RETURN( m_lightManager.Initialize( g_gameObjects ) );
	m_shadowManager.Init( *gRenderer );

	return true;
}

void CGameLogic::UpdateLogic ( void )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	CTimer::GetInstance().Tick();

	StartLogic ();
	ProcessLogic ();
	EndLogic ();

	if ( showFps.GetBool( ) )
	{
		String fps = _T( "FPS : " ) + TO_STRING( CTimer::GetInstance().GetFps() );
		SetWindowText( m_wndHwnd, fps.c_str( ) );
	}
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
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		HandleWIndowMouseInput( msg.message, msg.wParam, msg.lParam );
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
						 break;
					 case VK_DOWN:
						 break;
					 case VK_RIGHT:
						 break;
					 case VK_LEFT:
						 break;
					 default:
						 break;
					 }
		}
	default:
		break;
	}
}

void CGameLogic::HandleWIndowMouseInput( const int message, const WPARAM wParam, const LPARAM lParam )
{
	CWinProcMouseInputTranslator translator;
	MOUSE_INPUT_INFO& input = translator.TranslateInput( message, wParam, lParam );

	m_mouseController.ProcessInput( input );
}

CGameLogic::CGameLogic( ):
	m_wndHwnd( nullptr )
{
	ShowDebugConsole( );
}

CON_COMMAND( print_objects_pos, "print all visible object position" )
{
	for ( const auto& object : g_gameObjects )
	{
		if ( object->ShouldDraw( ) )
		{
			DebugMsg( "name - %s\n", object->GetName( ).c_str() );
			DebugMsg( "pos - %f %f %f\n", object->GetPosition( ).x, object->GetPosition( ).y, object->GetPosition( ).z );
		}
	}
}
