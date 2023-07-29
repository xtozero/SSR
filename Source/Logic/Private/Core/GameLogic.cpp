#include "stdafx.h"
#include "Core/GameLogic.h"

#include "common.h"
#include "Components/CameraComponent.h"
#include "Config/DefaultLogicConfig.h"
#include "ConsoleMessage/ConCommand.h"
#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "ConsoleMessage/ConVar.h"
#include "Core/DebugConsole.h"
#include "Core/Timer.h"
#include "Core/UtilWindowInfo.h"
#include "FileSystem.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameClientViewport.h"
#include "IAgl.h"
#include "InterfaceFactories.h"
#include "Json/json.hpp"
#include "Platform/IPlatform.h"
#include "Renderer/IRenderCore.h"
#include "Scene/IScene.h"
//#include "Render/IRenderResourceManager.h"
#include "UserInput/UserInput.h"
#include "UserInterfaceRenderer.h"
#include "World/WorldLoader.h"

#include <cstddef>
#include <ctime>
#include <tchar.h>

namespace
{
	ConVar( showFps, "1", "Show Fps" );

	void WaitRenderThreadForShutdown()
	{
		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>(
			[]()
			{
				GetInterface<agl::IAgl>()->WaitGPU();
				GetInterface<agl::IAgl>()->OnShutdown();
			} );
		GetInterface<ITaskScheduler>()->Wait( handle );
	}
}

bool CGameLogic::BootUp( IPlatform& platform )
{
	m_renderCoreDll = LoadModule( "RenderCore.dll" );
	if ( m_renderCoreDll == nullptr )
	{
		return false;
	}

	m_pRenderCore = GetInterface<rendercore::IRenderCore>();
	if ( m_pRenderCore == nullptr )
	{
		return false;
	}

	m_wndHwnd = platform.GetRawHandle<HWND>();
	srand( static_cast<uint32>( time( nullptr ) ) );

	m_appSize = platform.GetSize();
	CUtilWindowInfo::GetInstance().SetRect( m_appSize.first, m_appSize.second );

	if ( m_pRenderCore->BootUp() == false )
	{
		return false;
	}

	m_inputController = std::make_unique<PlayerController>();

	//m_view.CreatePerspectiveFovLHMatrix( XMConvertToRadians( 60 ),
	//	static_cast<float>( m_appSize.first ) / m_appSize.second,
	//	1.f,
	//	1500.f );

	/*
	m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
		static_cast<float>( m_appSize.first ) / m_appSize.second,
		1.f,
		1500.f );

	m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );
	*/

	m_uiProjMat = Matrix(
		2.0f / m_appSize.first,	0.f,						0.f,	0.f,
		0.f,					-2.0f / m_appSize.second,	0.f,	0.f,
		0.f,					0.f,						0.5f,	0.f,
		-1.f,					1.f,						0.5f,	1.f );

	// m_pickingManager.PushCamera( &GetLocalPlayer()->GetCamera() );
	// m_inputBroadCaster.AddListener( &m_pickingManager );

	CreateGameViewport();

	if ( LoadWorld( DefaultLogic::GetDefaultWorld() ) == false )
	{
		__debugbreak();
	}

	//if ( m_ssrManager.Init( *this ) == false )
	//{
	//	__debugbreak( );
	//}

	//if ( m_ui.Initialize( ) == false )
	//{
	//	__debugbreak( );
	//}

	if ( m_debugOverlay.Init( *this ) == false )
	{
		__debugbreak();
	}

	m_world.Initialize();

	return CreateDeviceDependentResource();
}

void CGameLogic::Update()
{
	// �� �������� ���� ElapsedTime ����
	StartLogic();
	ProcessLogic();
	EndLogic();
}

void CGameLogic::Pause()
{
	m_world.Pause();
}

void CGameLogic::Resume()
{
	m_world.Resume();
}

void CGameLogic::HandleUserInput( const UserInput& input )
{
	if ( m_inputController == nullptr )
	{
		return;
	}

	m_inputController->ProcessInput( input );
}

void CGameLogic::HandleTextInput( uint64 text, bool bUnicode )
{
}

void CGameLogic::AppSizeChanged( IPlatform& platform )
{
	const std::pair<uint32, uint32>& newAppSize = platform.GetSize();

	if ( m_appSize == newAppSize )
	{
		return;
	}

	m_appSize = newAppSize;
	CUtilWindowInfo::GetInstance().SetRect( m_appSize.first, m_appSize.second );

	m_gameViewport->AppSizeChanged( platform.GetRawHandle<void*>(), newAppSize );

	//m_pRenderer->AppSizeChanged( m_appSize.first, m_appSize.second );

	//m_ssrManager.AppSizeChanged( *this );

	float fSizeX = static_cast<float>( m_appSize.first );
	float fSizeY = static_cast<float>( m_appSize.second );

	//m_view.CreatePerspectiveFovLHMatrix( XMConvertToRadians( 60 ),
	//	fSizeX / fSizeY,
	//	1.f,
	//	1500.f );

	/*
	m_pickingManager.PopInvProjection( );
	m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
		fSizeX / fSizeY,
		1.f,
		1500.f );

	m_pickingManager.PopViewport( );
	m_pickingManager.PushViewport( 0.0f, 0.0f, fSizeX, fSizeY );
	*/

	m_uiProjMat = Matrix(
		2.0f / fSizeX,	0.f,			0.f,	0.f,
		0.f,			-2.0f / fSizeY,	0.f,	0.f,
		0.f,			0.f,			0.5f,	0.f,
		-1.f,			1.f,			0.5f,	1.f );
}

void CGameLogic::SpawnObject( Owner<CGameObject*> object )
{
	m_world.SpawnObject( *this, object );
}

InputController* CGameLogic::GetInputController()
{
	return m_inputController.get();
}

void CGameLogic::Shutdown()
{
	WaitRenderThreadForShutdown();

	m_world.CleanUp();
	m_primayViewport.reset();

	ShutdownModule( m_renderCoreDll );
}

void CGameLogic::StartLogic()
{
	//���� ���� ���� ��ó��
	GetConsoleMessageExecutor().Execute();

	m_world.BeginFrame();

	//Rect clientRect( 0.f, 0.f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );
	//m_ui.BeginFrame( clientRect, m_clock.GetElapsedTime(), m_clock.GetTotalTime() );

	//if ( showFps.GetBool( ) )
	//{
	//	m_ui.Window( "FPS Window" );
	//	std::string fps = std::string( "FPS : " ) + std::to_string( m_clock.GetFps( ) );
	//	m_ui.Text( fps.c_str( ) );
	//	m_ui.EndWindow( );
	//}
}

void CGameLogic::ProcessLogic()
{
	if ( m_world.GetTimer().IsPaused() == false )
	{
		// ���� ���� ����
		m_world.RunFrame();
	}
}

void CGameLogic::EndLogic()
{
	// ���� �ùķ��̼� ����� �ݿ�
	m_world.EndFrame();

	//// ���� ���� ���� ��ó��
	//BuildRenderableList( );

	//CameraComponent& playerCamera = GetLocalPlayer( )->GetCamera( );
	//playerCamera.UpdateToRenderer( m_view );

	//m_view.UpdataView( *this, m_commonConstantBuffer[VS_VIEW_PROJECTION] );

	//float wndWidth = static_cast<float>( m_appSize.first );
	//float wndHeight = static_cast<float>( m_appSize.second );
	//Viewport viewport = { 0.f, 0.f, wndWidth, wndHeight, 0.f, 1.f };
	//m_view.SetViewPort( *m_pRenderer, &viewport, 1 );

	//RECT wndRect = { 0L, 0L, static_cast<LONG>( wndWidth ), static_cast<LONG>( wndHeight ) };
	//m_view.SetScissorRects( *m_pRenderer, &wndRect, 1 );
	//
	//if ( PassConstant* pData = static_cast<PassConstant*>( m_pRenderer->LockBuffer( m_commonConstantBuffer[PS_UTIL] ) ) )
	//{
	//	pData->m_receiversFar = m_view.GetFar();
	//	pData->m_receiversNear = m_view.GetNear( );
	//	pData->m_elapsedTime = m_clock.GetElapsedTime( );
	//	pData->m_totalTime = m_clock.GetTotalTime( );
	//	pData->m_renderTargetSize.x = wndWidth;
	//	pData->m_renderTargetSize.y = wndHeight;
	//	pData->m_invRenderTargetSize.x = 1.f / wndWidth;
	//	pData->m_invRenderTargetSize.y = 1.f / wndHeight;

	//	m_pRenderer->UnLockBuffer( m_commonConstantBuffer[PS_UTIL] );
	//	m_pRenderer->BindConstantBuffer( SHADER_TYPE::PS, static_cast<int32>( PS_CONSTANT_BUFFER::UTIL ), 1, &m_commonConstantBuffer[PS_UTIL] );
	//}

	//// �ĸ� ���� ������
	//m_ssrManager.PreProcess( *this, m_renderableList );

	//InitView( views );

	if ( m_numDrawRequestQueued == 0 )
	{
		++m_numDrawRequestQueued;
		UpdateUIDrawInfo();
		DrawScene();

		EnqueueRenderTask( [this]()
			{
				--m_numDrawRequestQueued;
			} );
	}

	//DrawForDebug( );
	//DrawDebugOverlay( );
	//SceneEnd( );
}

bool CGameLogic::LoadWorld( const char* filePath )
{
	IFileSystem* fileSystem = GetInterface<IFileSystem>();
	FileHandle worldAsset = fileSystem->OpenFile( filePath );

	if ( worldAsset.IsValid() == false )
	{
		return false;
	}

	unsigned long fileSize = fileSystem->GetFileSize( worldAsset );
	char* buffer = new char[fileSize];

	IFileSystem::IOCompletionCallback ParseWorldAsset;
	ParseWorldAsset.BindFunctor(
		[this, worldAsset]( const char* buffer, unsigned long bufferSize )
	{
		CWorldLoader::Load( *this, buffer, static_cast<size_t>( bufferSize ) );
		GetInterface<IFileSystem>()->CloseFile( worldAsset );
	}
	);

	bool result = fileSystem->ReadAsync( worldAsset, buffer, fileSize, &ParseWorldAsset );
	if ( result == false )
	{
		delete[] buffer;
		GetInterface<IFileSystem>()->CloseFile( worldAsset );
	}

	return result;
}

void CGameLogic::DrawScene()
{
	rendercore::IScene* scene = m_world.Scene();
	if ( scene == nullptr )
	{
		return;
	}

	EnqueueRenderTask( [scene]()
		{
			scene->OnBeginSceneRendering();
		} );

	m_gameViewport->Draw();
}

void CGameLogic::DrawForDebug()
{
	//m_ui.Window( "Debug Control" );

	//static bool colliderWireFrame = false;
	//colliderWireFrame = m_ui.Button( "Draw collider wireframe" ) ? !colliderWireFrame : colliderWireFrame;

	//if ( colliderWireFrame )
	//{
	//	for ( auto& object : m_gameObjects )
	//	{
	//		const ICollider* collider = object->GetDefaultCollider( );
	//		if ( collider )
	//		{
	//			collider->DrawDebugOverlay( m_debugOverlay, object->GetRigidBody()->IsAwake() ? g_colorChartreuse : g_colorRed, m_clock.GetElapsedTime() );
	//		}
	//	}
	//}

	//m_ui.EndWindow( );
}

void CGameLogic::DrawDebugOverlay()
{
	//m_debugOverlay.DrawPrimitive( *m_pRenderer, m_clock.GetElapsedTime() );
}

void CGameLogic::UpdateUIDrawInfo()
{
	auto uiRenderer = GetInterface<rendercore::UserInterfaceRenderer>();
	if ( uiRenderer )
	{
		uiRenderer->UpdateUIDrawInfo();
	}
}

void CGameLogic::SceneEnd()
{
	//BYTE errorCode = m_pRenderer->SceneEnd( );
	//if ( errorCode == DEVICE_ERROR::DEVICE_LOST )
	//{
	//	HandleDeviceLost( );
	//}
}

void CGameLogic::BuildRenderableList()
{
	//for ( auto& list : m_renderableList )
	//{
	//	list.clear();
	//}

	//for ( auto& object : m_gameObjects )
	//{
	//	if ( object )
	//	{
	//		// Reflectable list
	//		if ( object->GetProperty( ) & REFLECTABLE_OBJECT )
	//		{
	//			m_renderableList[REFLECT_RENDERABLE].push_back( object.get() );
	//		}

	//		// Opaque list
	//		if ( object->ShouldDraw( ) )
	//		{
	//			m_renderableList[OPAQUE_RENDERABLE].push_back( object.get( ) );
	//		}
	//	}
	//}
}

void CGameLogic::DrawReflectRenderable()
{
	//m_pRenderer->ForwardRenderEnd( );
	//m_ssrManager.Process( *this, m_renderableList );
}

void CGameLogic::HandleDeviceLost()
{
	//m_pRenderer->HandleDeviceLost( m_wndHwnd, m_appSize.first, m_appSize.second );

	//for ( auto& uiDrawBuffer : m_uiDrawBuffer )
	//{
	//	uiDrawBuffer.m_prevBufferSize = 0;
	//	uiDrawBuffer.m_buffer = RE_HANDLE::InValidHandle( );
	//}

	//CreateDeviceDependentResource( );

	//m_modelManager.OnDeviceRestore( *this );
	//m_ssrManager.OnDeviceRestore( *this );
	//m_debugOverlay.OnDeviceRestore( *this );
	//m_atmosphereManager.OnDeviceRestore( *this );

	m_world.OnDeviceRestore( *this );
}

bool CGameLogic::CreateDeviceDependentResource()
{
	//BufferTrait trait = { sizeof( GeometryTransform ),
	//	1,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
	//	RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
	//	0,
	//	nullptr,
	//	0,
	//	0 };

	//using namespace SHARED_CONSTANT_BUFFER;

	//m_commonConstantBuffer[VS_GEOMETRY] = m_pRenderer->CreateBuffer( trait );
	//if ( m_commonConstantBuffer[VS_GEOMETRY] == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//trait.m_stride = sizeof( ViewProjectionTrasform );

	//m_commonConstantBuffer[VS_VIEW_PROJECTION] = m_pRenderer->CreateBuffer( trait );
	//if ( m_commonConstantBuffer[VS_VIEW_PROJECTION] == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//trait.m_stride = sizeof( SurfaceTrait );

	//m_commonConstantBuffer[PS_SURFACE] = m_pRenderer->CreateBuffer( trait );
	//if ( m_commonConstantBuffer[PS_SURFACE] == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//trait.m_stride = sizeof( PassConstant );

	//m_commonConstantBuffer[PS_UTIL] = m_pRenderer->CreateBuffer( trait );
	//if ( m_commonConstantBuffer[PS_UTIL] == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//m_uiMaterial = m_pRenderer->SearchMaterial( "mat_draw_ui" );
	//if ( m_uiMaterial == INVALID_MATERIAL )
	//{
	//	return false;
	//}

	//if ( CreateDefaultFontResource( ) == false )
	//{
	//	return false;
	//}

	return true;
}

bool CGameLogic::CreateDefaultFontResource()
{
	//constexpr char* defaultFontData = "./Scripts/Fontdata.txt";

	//// Load default font
	//std::ifstream data( defaultFontData );
	//if ( data.good( ) == false )
	//{
	//	return false;
	//}

	//std::string atlasPath;
	//data >> atlasPath;

	//CTextAtlas defaultText;

	//IResourceManager& resourceMgr = m_pRenderer->GetResourceManager( );
	//defaultText.m_texture = resourceMgr.CreateShaderResourceFromFile( atlasPath );

	//if ( defaultText.m_texture == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//data >> defaultText.m_fontHeight;

	//FontUV fontInfo = { { 0.f, 0.f },{ 0.f, 1.0f }, 0.f };
	//int32 asciicode;

	//while ( data )
	//{
	//	data >> asciicode;
	//	data >> fontInfo.m_u.x;
	//	data >> fontInfo.m_u.y;
	//	data >> fontInfo.m_size;

	//	defaultText.m_fontInfo.emplace( static_cast<char>( asciicode ), fontInfo );
	//}

	//defaultText.m_displayOffset.y = 4.f;
	//defaultText.m_fontSpacing = 1.f;

	//m_ui.SetDefaultText( "default", defaultText );

	return true;
}

CGameLogic::CGameLogic() // : m_pickingManager( &m_gameObjects )
{
	// for ( int32 i = 0; i < SHARED_CONSTANT_BUFFER::Count; ++i )
	// {
	// 	m_commonConstantBuffer[i] = RE_HANDLE::InValidHandle( );
	// }
}

CGameLogic::~CGameLogic()
{
	Shutdown();
}

void CGameLogic::CreateGameViewport()
{
	m_primayViewport = std::make_unique<rendercore::Viewport>(
		m_appSize.first,
		m_appSize.second,
		m_wndHwnd,
		agl::ResourceFormat::R8G8B8A8_UNORM_SRGB );

	m_gameViewport = new GameClientViewport( m_primayViewport.get() );
	SpawnObject( m_gameViewport );
}

Owner<ILogic*> CreateGameLogic()
{
	return new CGameLogic();
}

void DestroyGameLogic( Owner<ILogic*> pGameLogic )
{
	delete pGameLogic;
}
