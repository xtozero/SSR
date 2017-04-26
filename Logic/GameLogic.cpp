#include "stdafx.h"
#include "CameraManager.h"
#include "DebugConsole.h"
#include "GameLogic.h"
#include "GameObject.h"
#include "Timer.h"
#include "UtilWindowInfo.h"

#include "../Engine/ConVar.h"
#include "../Engine/ConCommand.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/BaseMesh.h"
#include "../RenderCore/DebugMesh.h"
#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCOre/IRenderView.h"
#include "../RenderCore/RenderCoreDllFunc.h"
#include "../shared/IPlatform.h"
#include "../shared/UserInput.h"
#include "../shared/Util.h"

#include <ctime>
#include <tchar.h>

using namespace DirectX;

namespace
{
	ConVar( showFps, "1", "Show Fps" );
}

bool CGameLogic::Initialize( IPlatform& platform )
{
	m_pRenderer = CreateDirect3D11Renderer( );

	if ( m_pRenderer == nullptr )
	{
		return false;
	}

	m_wndHwnd = platform.GetRawHandle<HWND>();
	srand( static_cast<UINT>(time( nullptr )) );
	
	m_wndSize = platform.GetSize( );
	CUtilWindowInfo::GetInstance( ).SetRect( m_wndSize.first, m_wndSize.second );

	ON_FAIL_RETURN( m_pRenderer->InitializeRenderer( m_wndHwnd, m_wndSize.first, m_wndSize.second ) );

	m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>( m_wndSize.first ), static_cast<float>( m_wndSize.second ) );

	IRenderView* view = m_pRenderer->GetCurrentRenderView( );

	if ( view )
	{
		view->CreatePerspectiveFovLHMatrix( XMConvertToRadians( 60 ),
			static_cast<float>( m_wndSize.first ) / m_wndSize.second,
			1.f,
			1500.0f );
		m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
			static_cast<float>( m_wndSize.first ) / m_wndSize.second,
			1.f,
			1500.0f );
	}
	else
	{
		return false;
	}

	m_pickingManager.PushCamera( &m_mainCamera );
	m_inputBroadCaster.AddListener( &m_pickingManager );
	m_inputBroadCaster.AddListener( &m_mainCamera );

	CCameraManager::GetInstance( )->SetCurrentCamera( &m_mainCamera );

	ON_FAIL_RETURN( LoadScene( ) );
	ON_FAIL_RETURN( m_lightManager.Initialize( *m_pRenderer, m_gameObjects ) );
	m_shadowManager.Init( *m_pRenderer );

	ON_FAIL_RETURN( m_ssrManager.Init( *m_pRenderer, m_pRenderer->GetMeshBuilder( ) ) );

	return true;
}

void CGameLogic::Update( void )
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

void CGameLogic::HandleUserInput( const UserInput& input )
{
	m_inputBroadCaster.ProcessInput( input );
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
	float wndWidth = static_cast<float>( m_wndSize.first );
	float wndHeight = static_cast<float>( m_wndSize.second );
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
		UTIL::Split( found->GetValue( ), param, ' ' );

		if ( param.size( ) == 3 )
		{
			m_mainCamera.SetOrigin( CXMFLOAT3( 
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

Owner<ILogic*> CreateGameLogic( )
{
	return new CGameLogic( );
}