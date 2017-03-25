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

bool CGameLogic::Initialize( HWND hwnd, UINT wndWidth, UINT wndHeight )
{
	m_pRenderer = CreateDirect3D11Renderer( );

	if ( m_pRenderer == nullptr )
	{
		return false;
	}

	m_wndHwnd = hwnd;
	srand( static_cast<UINT>(time( nullptr )) );
	CUtilWindowInfo::GetInstance( ).SetRect( wndWidth, wndHeight );

	ON_FAIL_RETURN( m_pRenderer->InitializeRenderer( hwnd, wndWidth, wndHeight ) );

	m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>(wndWidth), static_cast<float>(wndHeight) );

	IRenderView* view = m_pRenderer->GetCurrentRenderView( );

	if ( view )
	{
		view->CreatePerspectiveFovLHMatrix( D3DXToRadian( 60 ),
			static_cast<float>(wndWidth) / wndHeight,
			1.f,
			1500.0f );
		m_pickingManager.PushInvProjection( D3DXToRadian( 60 ),
			static_cast<float>(wndWidth) / wndHeight,
			1.f,
			1500.0f );
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
	ON_FAIL_RETURN( m_lightManager.Initialize( *m_pRenderer, m_gameObjects ) );
	m_shadowManager.Init( *m_pRenderer );

	ON_FAIL_RETURN( m_ssrManager.Init( *m_pRenderer, m_pRenderer->GetMeshBuilder( ) ) );

	return true;
}

void CGameLogic::UpdateLogic( void )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	CTimer::GetInstance( ).Tick( );

	StartLogic( );
	ProcessLogic( );
	EndLogic( );

	if ( showFps.GetBool( ) )
	{
		String fps = _T( "FPS : " ) + TO_STRING( CTimer::GetInstance( ).GetFps( ) );
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

void CGameLogic::StartLogic ( void )
{
	//게임 로직 수행 전처리
}

void CGameLogic::ProcessLogic ( void )
{
	//게임 로직 수행
	for ( auto& object : m_gameObjects )
	{
		object->Think( );
	}
}

void CGameLogic::EndLogic ( void )
{
	//그림자 맵 렌더링
	m_shadowManager.Process( m_lightManager, *m_pRenderer, m_gameObjects );

	//게임 로직 수행 후처리
	m_mainCamera.UpdateToRenderer( *m_pRenderer );
	m_lightManager.UpdateToRenderer( *m_pRenderer, m_mainCamera );
	BuildRenderableList( );
	SceneBegin( );
	DrawScene( );
	SceneEnd( );
}

bool CGameLogic::LoadScene( void )
{
	m_gameObjects.erase( m_gameObjects.begin( ), m_gameObjects.end( ) );
	auto keyValue = m_sceneLoader.LoadSceneFromFile( *m_pRenderer, m_gameObjects, _T( "../Script/TestScene.txt" ) );

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
	m_pRenderer->PushViewPort( 0.f, 0.f, wndWidth, wndHeight );
	m_pRenderer->PushScissorRect( CUtilWindowInfo::GetInstance( ).GetRect() );

	m_pRenderer->SceneBegin( );
}

void CGameLogic::DrawScene( void ) const
{
	DrawOpaqueRenderable( );
	DrawTransparentRenderable( );
	DrawReflectRenderable( );
}

void CGameLogic::SceneEnd( void ) const
{
	m_pRenderer->SceneEnd( );
}

void CGameLogic::UpdateWorldMatrix( CGameObject* object ) const
{
	if ( object )
	{
		m_pRenderer->UpdateWorldMatrix( object->GetTransformMatrix( ), object->GetInvTransformMatrix( ) );
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
			m_mainCamera.SetOrigin( XMFLOAT3( 
				static_cast<float>( _ttof( param[0].c_str( ) ) ),
				static_cast<float>( _ttof( param[1].c_str( ) ) ),
				static_cast<float>( _ttof( param[2].c_str( ) ) ) ) );
		}
	}
}

void CGameLogic::BuildRenderableList( )
{
	for ( auto& list : m_renderableList )
	{
		list.clear( );
	}

	for ( auto& object : m_gameObjects )
	{
		if ( object )
		{
			// Reflectable list
			if ( object->GetProperty( ) & REFLECTABLE_OBJECT )
			{
				m_renderableList[REFLECT_RENDERABLE].push_back( object.get() );
			}

			// Opaque list
			if ( object->ShouldDraw( ) )
			{
				m_renderableList[OPAQUE_RENDERABLE].push_back( object.get( ) );
			}
		}
	}
}

void CGameLogic::DrawOpaqueRenderable( ) const
{
	for ( auto& object : m_renderableList[OPAQUE_RENDERABLE] )
	{
		UpdateWorldMatrix( object );
		object->Render( *m_pRenderer );
	}
}

void CGameLogic::DrawTransparentRenderable( ) const
{
	for ( auto& object : m_renderableList[TRANSPARENT_RENDERABLE] )
	{
		UpdateWorldMatrix( object );
		object->Render( *m_pRenderer );
	}
}

void CGameLogic::DrawReflectRenderable( ) const
{
	m_pRenderer->ForwardRenderEnd( );
	m_ssrManager.Process( *m_pRenderer, m_renderableList[REFLECT_RENDERABLE] );
}

CGameLogic::CGameLogic( ):
	m_wndHwnd( nullptr ),
	m_pickingManager( &m_gameObjects ),
	m_pRenderer( nullptr )
{
	ShowDebugConsole( );
}