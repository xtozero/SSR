#include "stdafx.h"
#include "Core/GameLogic.h"

#include "ConsoleMessage/ConVar.h"
#include "ConsoleMessage/ConCommand.h"
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

#include <ctime>
#include <tchar.h>

using namespace DirectX;

namespace
{
	ConVar( showFps, "1", "Show Fps" );
}

bool CGameLogic::Initialize( IPlatform& platform )
{
	HMODULE renderCoreDll = LoadLibrary( _T( "../bin/RenderCore.dll" ) );
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

	m_uiProjMat = {
		2.0f / m_appSize.first,	0.f,						0.f,	0.f,
		0.f,					-2.0f / m_appSize.second,	0.f,	0.f,
		0.f,					0.f,						0.5f,	0.f,
		-1.f,					1.f,						0.5f,	1.f
	};

	m_pickingManager.PushCamera( &m_mainCamera );
	m_inputBroadCaster.AddListener( &m_pickingManager );
	m_inputBroadCaster.AddListener( &m_mainCamera );
	CCameraManager::GetInstance( ).SetCurrentCamera( &m_mainCamera );

	if ( LoadScene( _T( "../Script/defaultScene.txt" ) ) == false )
	{
		__debugbreak( );
	}
	
	if ( m_lightManager.Initialize( *m_pRenderer, m_gameObjects ) == false )
	{
		__debugbreak( );
	}

	m_shadowManager.Init( *this );

	if ( m_ssrManager.Init( *this ) == false )
	{
		__debugbreak( );
	}

	if ( m_ui.Initialize( ) == false )
	{
		__debugbreak( );
	}

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
	if ( m_ui.HandleUserInput( input ) == false )
	{
		m_inputBroadCaster.ProcessInput( input );
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
}

void CGameLogic::StartLogic( )
{
	//게임 로직 수행 전처리
	Rect clientRect( 0.f, 0.f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );
	m_ui.BeginFrame( clientRect );

	m_ui.Window( "FPS Window" );
	if ( showFps.GetBool( ) )
	{
		std::string fps = std::string( "FPS : " ) + std::to_string( CTimer::GetInstance( ).GetFps( ) );
		m_ui.Text( fps.c_str( ) );
	}
	m_ui.EndWindow( );
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
	using namespace SHARED_CONSTANT_BUFFER;

	//게임 로직 수행 후처리
	m_mainCamera.UpdateToRenderer( m_view );
	m_lightManager.UpdateToRenderer( *m_pRenderer, m_mainCamera );

	m_shadowManager.BuildShadowProjectionMatrix( *this, m_gameObjects );

	m_view.UpdataView( *this, m_commonConstantBuffer[VS_VIEW_PROJECTION] );

	// 그림자 맵 렌더링
	m_shadowManager.DrawShadowMap( *this, m_gameObjects );

	float wndWidth = static_cast<float>( m_appSize.first );
	float wndHeight = static_cast<float>( m_appSize.second );
	Viewport viewport = { 0.f, 0.f, wndWidth, wndHeight, 0.f, 1.f };
	m_view.SetViewPort( *m_pRenderer, &viewport, 1 );
	m_view.SetScissorRects( *m_pRenderer, &CUtilWindowInfo::GetInstance( ).GetRect( ), 1 );
	
	if ( PassConstant* pData = static_cast<PassConstant*>( m_pRenderer->LockBuffer( m_commonConstantBuffer[PS_UTIL] ) ) )
	{
		pData->m_receiversFar = m_view.GetFar();
		pData->m_receiversNear = m_view.GetNear( );
		pData->m_elapsedTime = CTimer::GetInstance( ).GetElapsedTIme( );
		pData->m_totalTime = CTimer::GetInstance( ).GetTotalTime( );
		pData->m_renderTargetSize.x = wndWidth;
		pData->m_renderTargetSize.y = wndHeight;
		pData->m_invRenderTargetSize.x = 1.f / wndWidth;
		pData->m_invRenderTargetSize.y = 1.f / wndHeight;

		m_pRenderer->UnLockBuffer( m_commonConstantBuffer[PS_UTIL] );
		m_pRenderer->BindConstantBuffer( SHADER_TYPE::PS, static_cast<int>( PS_CONSTANT_BUFFER::UTIL ), 1, &m_commonConstantBuffer[PS_UTIL] );
	}

	// 후면 깊이 렌더링
	m_ssrManager.PreProcess( *this, m_renderableList );

	BuildRenderableList( );
	SceneBegin( );
	DrawScene( );
	DrawUI( );
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

void CGameLogic::SceneBegin( )
{
	m_pRenderer->SceneBegin( );
}

void CGameLogic::DrawScene( )
{
	DrawOpaqueRenderable( );
	DrawTransparentRenderable( );
	DrawReflectRenderable( );
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
			if ( drawBuffer.m_buffer != RE_HANDLE_TYPE::INVALID_HANDLE )
			{
				resourceMgr.FreeResource( drawBuffer.m_buffer );
			}
		}

		BUFFER_TRAIT trait = { sizeof( ImUiVertex ),
			static_cast<UINT>( drawData.m_totalVertexCount ),
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
			RESOURCE_TYPE::VERTEX_BUFFER,
			0U,
			nullptr,
			0U,
			0U
		};

		m_uiDrawBuffer[0].m_buffer = m_pRenderer->CreateBuffer( trait );
		m_uiDrawBuffer[0].m_prevBufferSize = drawData.m_totalVertexCount;

		trait.m_stride = sizeof( DWORD );
		trait.m_count = drawData.m_totalIndexCount;
		trait.m_bufferType = RESOURCE_TYPE::INDEX_BUFFER,

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
			int copySize = sizeof( ImUiVertex ) * drawList->m_vertices.size( );
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

	int indexOffset = 0;
	int vertexOffset = 0;
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

		vertexOffset += drawList->m_vertices.size( );
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

	CreateDeviceDependentResource( );

	m_meshManager.OnDeviceRestore( *this );
	m_lightManager.OnDeviceRestore( *this );
	m_shadowManager.OnDeviceRestore( *this );
	m_ssrManager.OnDeviceRestore( *this );
	m_mainCamera.OnDeviceRestore( *this );

	for ( auto& object : m_gameObjects )
	{
		object->OnDeviceRestore( *this );
	}
}

bool CGameLogic::CreateDeviceDependentResource( )
{
	BUFFER_TRAIT trait = { sizeof( GeometryTransform ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_TYPE::CONSTANT_BUFFER,
		0,
		nullptr,
		0,
		0 };

	using namespace SHARED_CONSTANT_BUFFER;

	m_commonConstantBuffer[VS_GEOMETRY] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[VS_GEOMETRY] == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	trait.m_stride = sizeof( ViewProjectionTrasform );

	m_commonConstantBuffer[VS_VIEW_PROJECTION] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[VS_VIEW_PROJECTION] == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	trait.m_stride = sizeof( SurfaceTrait );

	m_commonConstantBuffer[PS_SURFACE] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[PS_SURFACE] == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	trait.m_stride = sizeof( PassConstant );

	m_commonConstantBuffer[PS_UTIL] = m_pRenderer->CreateBuffer( trait );
	if ( m_commonConstantBuffer[PS_UTIL] == RE_HANDLE_TYPE::INVALID_HANDLE )
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
	constexpr char* defaultFontData = "../Script/Fontdata.txt";

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

	if ( defaultText.m_texture == RE_HANDLE_TYPE::INVALID_HANDLE )
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

CGameLogic::CGameLogic( ) : m_pickingManager( &m_gameObjects )
{
	for ( int i = 0; i < SHARED_CONSTANT_BUFFER::Count; ++i )
	{
		m_commonConstantBuffer[i] = RE_HANDLE_TYPE::INVALID_HANDLE;
	}

	ShowDebugConsole( );
}

Owner<ILogic*> CreateGameLogic( )
{
	return new CGameLogic( );
}