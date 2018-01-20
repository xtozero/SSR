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
#include "../RenderCore/CommonRenderer/IRenderer.h"
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
	m_pRenderer.reset( CreateDirect3D11Renderer( ) );

	if ( m_pRenderer == nullptr )
	{
		return false;
	}

	m_wndHwnd = platform.GetRawHandle<HWND>();
	srand( static_cast<UINT>(time( nullptr )) );
	
	m_appSize = platform.GetSize( );
	CUtilWindowInfo::GetInstance( ).SetRect( m_appSize.first, m_appSize.second );

	ON_FAIL_RETURN( m_pRenderer->BootUp( m_wndHwnd, m_appSize.first, m_appSize.second ) );

	m_view.CreatePerspectiveFovLHMatrix( XMConvertToRadians( 60 ),
		static_cast<float>( m_appSize.first ) / m_appSize.second,
		1.f,
		1500.0f );

	m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
		static_cast<float>( m_appSize.first ) / m_appSize.second,
		1.f,
		1500.0f );

	m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );

	m_pickingManager.PushCamera( &m_mainCamera );
	m_inputBroadCaster.AddListener( &m_pickingManager );
	m_inputBroadCaster.AddListener( &m_mainCamera );
	CCameraManager::GetInstance( ).SetCurrentCamera( &m_mainCamera );

	ON_FAIL_RETURN( m_view.initialize( *m_pRenderer ) );

	ON_FAIL_RETURN( LoadScene( _T( "../Script/defaultScene.txt" ) ) );
	ON_FAIL_RETURN( m_lightManager.Initialize( *m_pRenderer, m_gameObjects ) );
	m_shadowManager.Init( *this );

	ON_FAIL_RETURN( m_ssrManager.Init( *this ) );

	return CreateDeviceDependentResource( );
}

void CGameLogic::Update( )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	CTimer::GetInstance( ).Tick( );

	m_mainCamera.Think( );

	StartLogic( );
	ProcessLogic( );
	EndLogic( );

	if ( showFps.GetBool( ) )
	{
		String fps = _T( "FPS : " ) + TO_STRING( CTimer::GetInstance( ).GetFps( ) );
		SetWindowText( m_wndHwnd, fps.c_str( ) );
	}
}

void CGameLogic::Pause( )
{
	CTimer::GetInstance().Pause( );
}

void CGameLogic::Resume( )
{
	CTimer::GetInstance().Resume( );
}

void CGameLogic::HandleUserInput( const UserInput& input )
{
	m_inputBroadCaster.ProcessInput( input );
}

void CGameLogic::AppSizeChanged( IPlatform& platform )
{
	const std::pair<UINT, UINT>& newAppSize = platform.GetSize( );

	if ( m_appSize == newAppSize )
	{
		return;
	}

	m_appSize = newAppSize;
	CUtilWindowInfo::GetInstance( ).SetRect( m_appSize.first, m_appSize.second );

	m_pRenderer->AppSizeChanged( m_appSize.first, m_appSize.second );

	m_ssrManager.AppSizeChanged( *this );

	m_view.CreatePerspectiveFovLHMatrix( XMConvertToRadians( 60 ),
		static_cast<float>( m_appSize.first ) / m_appSize.second,
		1.f,
		1500.0f );

	m_pickingManager.PopInvProjection( );
	m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
		static_cast<float>( m_appSize.first ) / m_appSize.second,
		1.f,
		1500.0f );

	m_pickingManager.PopViewport( );
	m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );
}

void CGameLogic::StartLogic( )
{
	//게임 로직 수행 전처리
}

void CGameLogic::ProcessLogic( )
{
	//게임 로직 수행
	for ( auto& object : m_gameObjects )
	{
		object->Think( );
	}
}

void CGameLogic::EndLogic( )
{
	//그림자 맵 렌더링
	m_shadowManager.Process( m_lightManager, *this, m_gameObjects );

	//게임 로직 수행 후처리
	m_mainCamera.UpdateToRenderer( m_view );
	m_lightManager.UpdateToRenderer( *m_pRenderer, m_mainCamera );
	BuildRenderableList( );
	SceneBegin( );
	DrawScene( );
	SceneEnd( );
}

bool CGameLogic::LoadScene( const String& scene )
{
	m_gameObjects.clear( );

	CSceneLoader sceneLoader;
	std::unique_ptr<KeyValue> keyValue = sceneLoader.LoadSceneFromFile( *this, m_gameObjects, scene.c_str() );

	if ( !keyValue )
	{
		return false;
	}

	m_mainCamera.LoadProperty( *keyValue );

	return true;
}

void CGameLogic::SceneBegin( void )
{
	float wndWidth = static_cast<float>( m_appSize.first );
	float wndHeight = static_cast<float>( m_appSize.second );
	m_view.PushViewPort( 0.f, 0.f, wndWidth, wndHeight );
	m_view.PushScissorRect( CUtilWindowInfo::GetInstance( ).GetRect() );
	m_view.SetViewPort( *m_pRenderer );
	m_view.SetScissorRects( *m_pRenderer );
	m_view.UpdataView( );

	m_pRenderer->SceneBegin( );
}

void CGameLogic::DrawScene( void )
{
	DrawOpaqueRenderable( );
	DrawTransparentRenderable( );
	DrawReflectRenderable( );
}

void CGameLogic::SceneEnd( void )
{
	BYTE errorCode = m_pRenderer->SceneEnd( );
	if ( errorCode == DEVICE_ERROR::DEVICE_LOST )
	{
		HandleDeviceLost( );
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

void CGameLogic::DrawOpaqueRenderable( )
{
	for ( auto& object : m_renderableList[OPAQUE_RENDERABLE] )
	{
		object->UpdateTransform( *this );
		object->Render( *this );
	}
}

void CGameLogic::DrawTransparentRenderable( )
{
	for ( auto& object : m_renderableList[TRANSPARENT_RENDERABLE] )
	{
		object->UpdateTransform( *this );
		object->Render( *this );
	}
}

void CGameLogic::DrawReflectRenderable( )
{
	m_pRenderer->ForwardRenderEnd( );
	m_ssrManager.Process( *this, m_renderableList[REFLECT_RENDERABLE] );
}

void CGameLogic::HandleDeviceLost( )
{
	m_pRenderer->HandleDeviceLost( m_wndHwnd, m_appSize.first, m_appSize.second );

	CreateDeviceDependentResource( );

	m_meshManager.OnDeviceRestore( *this );
	m_lightManager.OnDeviceRestore( *this );
	m_shadowManager.OnDeviceRestore( *this );
	m_ssrManager.OnDeviceRestore( *this );
	m_view.OnDeviceRestore( *this );
	m_mainCamera.OnDeviceRestore( *this );

	for ( auto& object : m_gameObjects )
	{
		object->OnDeviceRestore( *this );
	}
}

bool CGameLogic::CreateDeviceDependentResource( )
{
	BUFFER_TRAIT trait = { sizeof( SurfaceTrait ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_TYPE::CONSTANT_BUFFER,
		0,
		nullptr,
		0,
		0 };

	using namespace SHARED_CONSTANT_BUFFER;

	m_commonConstantBuffer[PS_SURFACE] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[PS_SURFACE] == nullptr )
	{
		return false;
	}

	trait.m_stride = sizeof( GeometryTransform );

	m_commonConstantBuffer[VS_GEOMETRY] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[VS_GEOMETRY] == nullptr )
	{
		return false;
	}

	trait.m_stride = sizeof( ShadowTransform );

	m_commonConstantBuffer[VS_SHADOW] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[VS_SHADOW] == nullptr )
	{
		return false;
	}

	return true;
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