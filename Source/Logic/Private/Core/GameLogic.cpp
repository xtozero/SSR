#include "stdafx.h"
#include "Core/GameLogic.h"

#include "ConsoleMessage/ConCommand.h"
#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "ConsoleMessage/ConVar.h"
#include "Core/DebugConsole.h"
#include "Core/Timer.h"
#include "Core/UtilWindowInfo.h"
#include "DataStructure/KeyValueReader.h"
#include "GameObject/CameraManager.h"
#include "GameObject/GameObject.h"
#include "Platform/IPlatform.h"
#include "Render/IRenderer.h"
#include "Render/IRenderResourceManager.h"
#include "UserInput/UserInput.h"
#include "Util.h"

#include <cstddef>
#include <ctime>
#include <tchar.h>

using namespace DirectX;

namespace
{
	ConVar( showFps, "1", "Show Fps" );
}

bool CGameLogic::Initialize( IPlatform& platform )
{
	HMODULE renderCoreDll = LoadLibrary( _T( "./Binaries/RenderCore.dll" ) );
	if ( renderCoreDll == nullptr )
	{
		return false;
	}

	using CreateRendererFunc = IRenderer* (*)( );
	CreateRendererFunc CreateDirect3D11Renderer = reinterpret_cast<CreateRendererFunc>( GetProcAddress( renderCoreDll, "CreateDirect3D11Renderer" ) );

	m_pRenderer.reset( CreateDirect3D11Renderer( ) );

	if ( m_pRenderer == nullptr )
	{
		return false;
	}

	m_wndHwnd = platform.GetRawHandle<HWND>();
	srand( static_cast<UINT>(time( nullptr )) );
	
	m_appSize = platform.GetSize( );
	CUtilWindowInfo::GetInstance( ).SetRect( m_appSize.first, m_appSize.second );

	if ( m_pRenderer->BootUp( m_wndHwnd, m_appSize.first, m_appSize.second ) == false )
	{
		__debugbreak( );
	}

	m_view.CreatePerspectiveFovLHMatrix( XMConvertToRadians( 60 ),
		static_cast<float>( m_appSize.first ) / m_appSize.second,
		1.f,
		1500.f );

	m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
		static_cast<float>( m_appSize.first ) / m_appSize.second,
		1.f,
		1500.f );

	m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );

	m_uiProjMat = CXMFLOAT4X4(
		2.0f / m_appSize.first,	0.f,						0.f,	0.f,
		0.f,					-2.0f / m_appSize.second,	0.f,	0.f,
		0.f,					0.f,						0.5f,	0.f,
		-1.f,					1.f,						0.5f,	1.f
	);

	m_pickingManager.PushCamera( &GetLocalPlayer()->GetCamera() );
	m_inputBroadCaster.AddListener( &m_pickingManager );
	CCameraManager::GetInstance( ).SetCurrentCamera( &GetLocalPlayer( )->GetCamera( ) );
	
	if ( m_lightManager.Initialize( *this ) == false )
	{
		__debugbreak( );
	}

	if ( LoadScene( _T( "./Scripts/DefaultScene.txt" ) ) == false )
	{
		__debugbreak( );
	}

	m_shadowManager.Init( *this );

	if ( m_ssrManager.Init( *this ) == false )
	{
		__debugbreak( );
	}

	if ( m_atmosphereManager.Init( *this ) == false )
	{
		__debugbreak( );
	}

	if ( m_ui.Initialize( ) == false )
	{
		__debugbreak( );
	}

	if ( m_debugOverlay.Init( *this ) == false )
	{
		__debugbreak( );
	}

	return CreateDeviceDependentResource( );
}

void CGameLogic::Update( )
{
	// 한 프레임의 시작 ElapsedTime 갱신
	m_clock.Tick( );

	for ( auto& player : m_players )
	{
		player.GetCamera( ).Think( m_clock.GetElapsedTime() );
	}

	StartLogic( );
	ProcessLogic( );
	EndLogic( );
}

void CGameLogic::Pause( )
{
	m_clock.Pause( );
}

void CGameLogic::Resume( )
{
	m_clock.Resume( );
}

void CGameLogic::HandleUserInput( const UserInput& input )
{
	if ( m_ui.HandleUserInput( input ) == false )
	{
		m_inputBroadCaster.ProcessInput( input, *this );
	}
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

	float fSizeX = static_cast<float>( m_appSize.first );
	float fSizeY = static_cast<float>( m_appSize.second );

	m_view.CreatePerspectiveFovLHMatrix( XMConvertToRadians( 60 ),
		fSizeX / fSizeY,
		1.f,
		1500.f );

	m_pickingManager.PopInvProjection( );
	m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
		fSizeX / fSizeY,
		1.f,
		1500.f );

	m_pickingManager.PopViewport( );
	m_pickingManager.PushViewport( 0.0f, 0.0f, fSizeX, fSizeY );

	m_uiProjMat = XMMatrixOrthographicLH( fSizeX, fSizeY, 0, 1 );

	m_uiProjMat = CXMFLOAT4X4(
		2.0f / fSizeX,	0.f,			0.f,	0.f,
		0.f,			-2.0f / fSizeY,	0.f,	0.f,
		0.f,			0.f,			0.5f,	0.f,
		-1.f,			1.f,			0.5f,	1.f
	);
}

void CGameLogic::SpawnObject( Owner<CGameObject*> object )
{
	object->Initialize( *this, m_gameObjects.size( ) );
	m_gameObjects.emplace_back( object );
}

void CGameLogic::OnObjectSpawned( CGameObject& object )
{
	const BoundingSphere* sphere = reinterpret_cast<const BoundingSphere*>( object.GetCollider( COLLIDER::SPHERE ) );

	if ( sphere == nullptr )
	{
		return;
	}

	m_world.OnObjectSpawned( object.GetRigidBody( ), *sphere );
}

void CGameLogic::StartLogic( )
{
	//게임 로직 수행 전처리
	GetConsoleMessageExecutor( ).Execute( );

	for ( auto object = m_gameObjects.begin(); object != m_gameObjects.end(); )
	{
		CGameObject* candidate = object->get();
		if ( candidate->WillRemove( ) )
		{
			m_world.OnObjectRemoved( candidate->GetRigidBody( ) );
			object = m_gameObjects.erase( object );
		}
		else
		{
			++object;
		}
	}

	Rect clientRect( 0.f, 0.f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );
	m_ui.BeginFrame( clientRect, m_clock.GetElapsedTime(), m_clock.GetTotalTime() );

	if ( showFps.GetBool( ) )
	{
		m_ui.Window( "FPS Window" );
		std::string fps = std::string( "FPS : " ) + std::to_string( m_clock.GetFps( ) );
		m_ui.Text( fps.c_str( ) );
		m_ui.EndWindow( );
	}
}

void CGameLogic::ProcessLogic( )
{
	if ( m_clock.IsPaused() == false )
	{
		// 게임 로직 수행
		for ( auto& object : m_gameObjects )
		{
			object->Think( m_clock.GetElapsedTime() );
		}

		const float SIMULATE_INTERVAL = 0.016f;
		m_remainPhysicsSimulateTime += m_clock.GetElapsedTime( );
		while ( m_remainPhysicsSimulateTime >= SIMULATE_INTERVAL )
		{
			m_world.StartFrame( );
			m_world.RunPhysics( SIMULATE_INTERVAL );
			m_remainPhysicsSimulateTime -= SIMULATE_INTERVAL;
		}
	}
}

void CGameLogic::EndLogic( )
{
	using namespace SHARED_CONSTANT_BUFFER;

	// 물리 시뮬레이션 결과를 반영
	for ( auto& object : m_gameObjects )
	{
		object->PostThink( m_clock.GetElapsedTime( ) );
	}

	// 게임 로직 수행 후처리
	BuildRenderableList( );

	CCamera& playerCamera = GetLocalPlayer( )->GetCamera( );
	playerCamera.UpdateToRenderer( m_view );
	m_lightManager.UpdateToRenderer( *m_pRenderer, playerCamera );

	m_shadowManager.BuildShadowProjectionMatrix( *this, m_gameObjects );

	m_view.UpdataView( *this, m_commonConstantBuffer[VS_VIEW_PROJECTION] );

	// 그림자 맵 렌더링
	m_shadowManager.DrawShadowMap( *this );

	float wndWidth = static_cast<float>( m_appSize.first );
	float wndHeight = static_cast<float>( m_appSize.second );
	Viewport viewport = { 0.f, 0.f, wndWidth, wndHeight, 0.f, 1.f };
	m_view.SetViewPort( *m_pRenderer, &viewport, 1 );

	RECT wndRect = { 0L, 0L, static_cast<LONG>( wndWidth ), static_cast<LONG>( wndHeight ) };
	m_view.SetScissorRects( *m_pRenderer, &wndRect, 1 );
	
	if ( PassConstant* pData = static_cast<PassConstant*>( m_pRenderer->LockBuffer( m_commonConstantBuffer[PS_UTIL] ) ) )
	{
		pData->m_receiversFar = m_view.GetFar();
		pData->m_receiversNear = m_view.GetNear( );
		pData->m_elapsedTime = m_clock.GetElapsedTime( );
		pData->m_totalTime = m_clock.GetTotalTime( );
		pData->m_renderTargetSize.x = wndWidth;
		pData->m_renderTargetSize.y = wndHeight;
		pData->m_invRenderTargetSize.x = 1.f / wndWidth;
		pData->m_invRenderTargetSize.y = 1.f / wndHeight;

		m_pRenderer->UnLockBuffer( m_commonConstantBuffer[PS_UTIL] );
		m_pRenderer->BindConstantBuffer( SHADER_TYPE::PS, static_cast<int>( PS_CONSTANT_BUFFER::UTIL ), 1, &m_commonConstantBuffer[PS_UTIL] );
	}

	// 후면 깊이 렌더링
	m_ssrManager.PreProcess( *this, m_renderableList );

	SceneBegin( );
	DrawScene( );
	DrawForDebug( );
	DrawDebugOverlay( );
	DrawUI( );
	SceneEnd( );
}

bool CGameLogic::LoadScene( const String& scene )
{
	CSceneLoader sceneLoader;
	std::unique_ptr<KeyValue> keyValue = sceneLoader.LoadSceneFromFile( *this, scene.c_str() );

	if ( !keyValue )
	{
		return false;
	}

	GetLocalPlayer()->GetCamera().LoadProperty( *keyValue );

	m_lightManager.SpawnLights( *this, m_gameObjects );

	return true;
}

void CGameLogic::ShutdownScene( )
{
	m_gameObjects.clear( );
}

void CGameLogic::SceneBegin( )
{
	m_pRenderer->SceneBegin( );
}

void CGameLogic::DrawScene( )
{
	CXMFLOAT3 sunDir( 0.f, 1.0f, 0.f );
	if ( CLight* pLight = m_lightManager.GetPrimaryLight( ) )
	{
		sunDir = -pLight->GetDirection( );
	}

	CCamera& playerCamera = GetLocalPlayer( )->GetCamera( );
	m_atmosphereManager.Render( GetRenderer( ), playerCamera.GetOrigin(), sunDir );
	m_shadowManager.PrepareBeforeRenderScene( GetRenderer() );

	DrawOpaqueRenderable( );
	DrawTransparentRenderable( );
	DrawReflectRenderable( );
}

void CGameLogic::DrawForDebug( )
{
	m_ui.Window( "Debug Control" );

	static bool colliderWireFrame = false;
	colliderWireFrame = m_ui.Button( "Draw collider wireframe" ) ? !colliderWireFrame : colliderWireFrame;

	if ( colliderWireFrame )
	{
		for ( auto& object : m_gameObjects )
		{
			const ICollider* collider = object->GetDefaultCollider( );
			if ( collider )
			{
				collider->DrawDebugOverlay( m_debugOverlay, object->GetRigidBody()->IsAwake() ? g_colorChartreuse : g_colorRed, m_clock.GetElapsedTime() );
			}
		}
	}

	m_ui.EndWindow( );
}

void CGameLogic::DrawDebugOverlay( )
{
	m_debugOverlay.DrawPrimitive( *m_pRenderer, m_clock.GetElapsedTime() );
}

void CGameLogic::DrawUI( )
{
	m_ui.EndFrame( );

	const ImDrawData drawData = m_ui.Render( );

	if ( drawData.m_drawLists.size( ) == 0 )
	{
		return;
	}

	IResourceManager& resourceMgr = m_pRenderer->GetResourceManager( );

	// Resize Buffer
	if ( m_uiDrawBuffer[0].m_prevBufferSize != drawData.m_totalVertexCount ||
		m_uiDrawBuffer[1].m_prevBufferSize != drawData.m_totalIndexCount )
	{
		for ( const ImUiDrawBuffer& drawBuffer : m_uiDrawBuffer )
		{
			if ( drawBuffer.m_buffer != RE_HANDLE::InValidHandle( ) )
			{
				resourceMgr.FreeResource( drawBuffer.m_buffer );
			}
		}

		BUFFER_TRAIT trait = { sizeof( ImUiVertex ),
			static_cast<UINT>( drawData.m_totalVertexCount ),
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
			RESOURCE_BIND_TYPE::VERTEX_BUFFER,
			0U,
			nullptr,
			0U,
			0U
		};

		m_uiDrawBuffer[0].m_buffer = m_pRenderer->CreateBuffer( trait );
		m_uiDrawBuffer[0].m_prevBufferSize = drawData.m_totalVertexCount;

		trait.m_stride = sizeof( DWORD );
		trait.m_count = drawData.m_totalIndexCount;
		trait.m_bindType = RESOURCE_BIND_TYPE::INDEX_BUFFER,

		m_uiDrawBuffer[1].m_buffer = m_pRenderer->CreateBuffer( trait );
		m_uiDrawBuffer[1].m_prevBufferSize = drawData.m_totalIndexCount;
	}

	BYTE* pVertex = reinterpret_cast<BYTE*>( m_pRenderer->LockBuffer( m_uiDrawBuffer[0].m_buffer ) );
	BYTE* pIndex = reinterpret_cast<BYTE*>( m_pRenderer->LockBuffer( m_uiDrawBuffer[1].m_buffer ) );

	if ( pVertex && pIndex )
	{
		// Update Vertex, Index Buffer
		for ( ImDrawList* drawList : drawData.m_drawLists )
		{
			std::size_t copySize = sizeof( ImUiVertex ) * drawList->m_vertices.size( );
			memcpy( pVertex, drawList->m_vertices.data( ), copySize );
			pVertex += copySize;

			copySize = sizeof( DWORD ) * drawList->m_indices.size( );
			memcpy( pIndex, drawList->m_indices.data( ), copySize );
			pIndex += copySize;
		}

		m_pRenderer->UnLockBuffer( m_uiDrawBuffer[0].m_buffer );
		m_pRenderer->UnLockBuffer( m_uiDrawBuffer[1].m_buffer );
	}
	else
	{
		__debugbreak( );
	}

	// Update Porjection Matrix
	using namespace SHARED_CONSTANT_BUFFER;

	ViewProjectionTrasform* viewProj = reinterpret_cast<ViewProjectionTrasform*>( m_pRenderer->LockBuffer( m_commonConstantBuffer[VS_VIEW_PROJECTION] ) );
	if ( viewProj )
	{
		viewProj->m_projection = XMMatrixTranspose( m_uiProjMat );
		
		m_pRenderer->UnLockBuffer( m_commonConstantBuffer[VS_VIEW_PROJECTION] );
	}
	else
	{
		__debugbreak( );
	}
	

	// Draw
	UINT stride = sizeof( ImUiVertex );
	UINT offset = 0;
	m_pRenderer->BindVertexBuffer( &m_uiDrawBuffer[0].m_buffer, 0, 1, &stride, &offset );
	m_pRenderer->BindIndexBuffer( m_uiDrawBuffer[1].m_buffer, 0 );

	m_pRenderer->BindMaterial( m_uiMaterial );

	UINT indexOffset = 0;
	UINT vertexOffset = 0;
	RECT scissorRect;
	for ( ImDrawList* drawList : drawData.m_drawLists )
	{
		for ( const ImDrawCmd& drawCmd : drawList->m_cmdBuffer )
		{
			const Rect& rc = drawCmd.m_clipRect;
			scissorRect = { static_cast<LONG>( rc.m_leftTop.x ),
							static_cast<LONG>( rc.m_leftTop.y ),
							static_cast<LONG>( rc.m_rightBottom.x ),
							static_cast<LONG>( rc.m_rightBottom.y ) };

			m_pRenderer->SetScissorRects( &scissorRect, 1 );
			m_pRenderer->BindShaderResource( SHADER_TYPE::PS, 0, 1, &drawCmd.m_textAtlas );
			m_pRenderer->DrawIndexed( RESOURCE_PRIMITIVE::TRIANGLELIST, drawCmd.m_indicesCount, indexOffset, vertexOffset );
			indexOffset += drawCmd.m_indicesCount;
		}

		assert( ( vertexOffset + drawList->m_vertices.size( ) ) <= UINT_MAX );
		vertexOffset += static_cast<UINT>( drawList->m_vertices.size( ) );
	}
}

void CGameLogic::SceneEnd( )
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
	m_ssrManager.Process( *this, m_renderableList );
}

void CGameLogic::HandleDeviceLost( )
{
	m_pRenderer->HandleDeviceLost( m_wndHwnd, m_appSize.first, m_appSize.second );

	for ( auto& uiDrawBuffer : m_uiDrawBuffer )
	{
		uiDrawBuffer.m_prevBufferSize = 0;
		uiDrawBuffer.m_buffer = RE_HANDLE::InValidHandle( );
	}

	CreateDeviceDependentResource( );

	m_meshManager.OnDeviceRestore( *this );
	m_lightManager.OnDeviceRestore( *this );
	m_shadowManager.OnDeviceRestore( *this );
	m_ssrManager.OnDeviceRestore( *this );
	m_debugOverlay.OnDeviceRestore( *this );
	m_atmosphereManager.OnDeviceRestore( *this );

	for ( auto& object : m_gameObjects )
	{
		object->OnDeviceRestore( *this );
	}

	for ( auto& player : m_players )
	{
		player.OnDeviceRestore( *this );
	}
}

bool CGameLogic::CreateDeviceDependentResource( )
{
	BUFFER_TRAIT trait = { sizeof( GeometryTransform ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
		0,
		nullptr,
		0,
		0 };

	using namespace SHARED_CONSTANT_BUFFER;

	m_commonConstantBuffer[VS_GEOMETRY] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[VS_GEOMETRY] == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	trait.m_stride = sizeof( ViewProjectionTrasform );

	m_commonConstantBuffer[VS_VIEW_PROJECTION] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[VS_VIEW_PROJECTION] == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	trait.m_stride = sizeof( SurfaceTrait );

	m_commonConstantBuffer[PS_SURFACE] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[PS_SURFACE] == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	trait.m_stride = sizeof( PassConstant );

	m_commonConstantBuffer[PS_UTIL] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[PS_UTIL] == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	m_uiMaterial = m_pRenderer->SearchMaterial( _T("mat_draw_ui") );
	if ( m_uiMaterial == INVALID_MATERIAL )
	{
		return false;
	}

	if ( CreateDefaultFontResource( ) == false )
	{
		return false;
	}

	return true;
}

bool CGameLogic::CreateDefaultFontResource( )
{
	constexpr char* defaultFontData = "./Scripts/Fontdata.txt";

	// Load default font
	Ifstream data( defaultFontData );
	if ( data.good( ) == false )
	{
		return false;
	}

	String atlasPath;
	data >> atlasPath;

	CTextAtlas defaultText;

	IResourceManager& resourceMgr = m_pRenderer->GetResourceManager( );
	defaultText.m_texture = resourceMgr.CreateShaderResourceFromFile( atlasPath );

	if ( defaultText.m_texture == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	data >> defaultText.m_fontHeight;

	FontUV fontInfo = { { 0.f, 0.f },{ 0.f, 1.0f }, 0.f };
	int asciicode;

	while ( data )
	{
		data >> asciicode;
		data >> fontInfo.m_u.x;
		data >> fontInfo.m_u.y;
		data >> fontInfo.m_size;

		defaultText.m_fontInfo.emplace( static_cast<char>( asciicode ), fontInfo );
	}

	defaultText.m_displayOffset.y = 4.f;
	defaultText.m_fontSpacing = 1.f;

	m_ui.SetDefaultText( "default", defaultText );

	return true;
}

CPlayer* CGameLogic::GetLocalPlayer( )
{
	if ( m_players.size( ) == 0 )
	{
		m_players.emplace_back( );
		m_inputBroadCaster.AddListener( &m_players.front() );
	}

	return &m_players.front();
}

CGameLogic::CGameLogic( ) : m_pickingManager( &m_gameObjects )
{
	for ( int i = 0; i < SHARED_CONSTANT_BUFFER::Count; ++i )
	{
		m_commonConstantBuffer[i] = RE_HANDLE::InValidHandle( );
	}
}

Owner<ILogic*> CreateGameLogic( )
{
	return new CGameLogic( );
}