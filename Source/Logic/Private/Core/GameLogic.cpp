#include "Core/GameLogic.h"

#include "common.h"
#include "Components/CameraComponent.h"
#include "Config/DefaultAppConfig.h"
#include "Config/DefaultLogicConfig.h"
#include "ConsoleMessage/ConCommand.h"
#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "ConsoleMessage/ConVar.h"
#include "Core/DebugConsole.h"
#include "Core/Timer.h"
#include "Core/UtilWindowInfo.h"
#include "CpuProfiler.h"
#include "DefaultAglConfig.h"
#include "FileSystem.h"
#include "GameObject/GameObject.h"
#include "IAgl.h"
#include "InterfaceFactories.h"
#include "Json/json.hpp"
#include "Platform/IPlatform.h"
#include "Renderer/IRenderCore.h"
#include "Scene/GameClientViewport.h"
#include "Scene/IScene.h"
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

namespace logic
{
	bool CGameLogic::BootUp( engine::IPlatform& platform )
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

		CreateGameViewport();

		m_inputController = std::make_unique<PlayerController>();

		/*
		m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
			static_cast<float>( m_appSize.first ) / m_appSize.second,
			1.f,
			1500.f );

		m_pickingManager.PushViewport( 0.0f, 0.0f, static_cast<float>( m_appSize.first ), static_cast<float>( m_appSize.second ) );
		*/

		// m_pickingManager.PushCamera( &GetLocalPlayer()->GetCamera() );

		if ( LoadWorld( DefaultLogic::GetDefaultWorld() ) == false )
		{
			__debugbreak();
		}

		//if ( m_ssrManager.Init( *this ) == false )
		//{
		//	__debugbreak( );
		//}

		if ( m_debugOverlay.Init( *this ) == false )
		{
			__debugbreak();
		}

		m_world.Initialize();

		return true;
	}

	void CGameLogic::Update()
	{
		CPU_PROFILE( Update );

		// 한 프레임의 시작 ElapsedTime 갱신
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

	void CGameLogic::HandleUserInput( const engine::UserInput& input )
	{
		if ( m_inputController == nullptr )
		{
			return;
		}

		m_inputController->ProcessInput( input );
	}

	void CGameLogic::HandleTextInput( [[maybe_unused]] uint64 text, [[maybe_unused]] bool bUnicode )
	{
	}

	void CGameLogic::AppSizeChanged( engine::IPlatform& platform )
	{
		const std::pair<uint32, uint32>& newAppSize = platform.GetSize();

		if ( m_appSize == newAppSize )
		{
			return;
		}

		m_appSize = newAppSize;
		CUtilWindowInfo::GetInstance().SetRect( m_appSize.first, m_appSize.second );

		if ( m_canvas->Handle() == platform.GetRawHandle<void*>() )
		{
			m_canvas->Resize( newAppSize );
		}

		//m_ssrManager.AppSizeChanged( *this );

		float fSizeX = static_cast<float>( m_appSize.first );
		float fSizeY = static_cast<float>( m_appSize.second );

		/*
		m_pickingManager.PopInvProjection( );
		m_pickingManager.PushInvProjection( XMConvertToRadians( 60 ),
			fSizeX / fSizeY,
			1.f,
			1500.f );

		m_pickingManager.PopViewport( );
		m_pickingManager.PushViewport( 0.0f, 0.0f, fSizeX, fSizeY );
		*/
	}

	GameClientViewport* CGameLogic::GetGameClientViewport()
	{
		return m_gameViewport.get();
	}

	bool CGameLogic::LoadWorld( const char* filePath )
	{
		UnloadWorld();

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

		if ( result && DefaultLogic::IsSaveLastWorldAsDefault() )
		{
			DefaultLogic::SetDefaultWorld( filePath );
		}

		return result;
	}

	void CGameLogic::UnloadWorld()
	{
		for ( auto& object : m_world.GameObjects() )
		{
			RemoveObject( *object );
		}
	}

	World& CGameLogic::GetWorld()
	{
		return m_world;
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
		m_canvas.reset();
		m_primayViewport.reset();

		ShutdownModule( m_renderCoreDll );
	}

	void CGameLogic::StartLogic()
	{
		CPU_PROFILE( StartLogic );

		//게임 로직 수행 전처리
		engine::GetConsoleMessageExecutor().Execute();

		m_world.BeginFrame();

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
		CPU_PROFILE( ProcessLogic );

		if ( m_world.GetTimer().IsPaused() == false )
		{
			// 게임 로직 수행
			m_world.RunFrame();
		}
	}

	void CGameLogic::EndLogic()
	{
		CPU_PROFILE( EndLogic );

		// 물리 시뮬레이션 결과를 반영
		m_world.EndFrame();

		// 게임 로직 수행 후처리


		//// 후면 깊이 렌더링
		//m_ssrManager.PreProcess( *this, m_renderableList );

		if ( m_numDrawRequestQueued < agl::DefaultAgl::GetBufferCount() )
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

		m_gameViewport->Draw( m_world, *m_canvas );
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

	CGameLogic::CGameLogic() // : m_pickingManager( &m_gameObjects )
	{
	}

	CGameLogic::~CGameLogic()
	{
		Shutdown();
	}

	void CGameLogic::CreateGameViewport()
	{
		if ( m_wndHwnd == nullptr )
		{
			return;
		}

		m_canvas = std::make_unique<rendercore::Canvas>(
			m_appSize.first,
			m_appSize.second,
			m_wndHwnd,
			agl::ResourceFormat::R8G8B8A8_UNORM_SRGB );

		if ( engine::DefaultApp::IsEditor() )
		{
			m_primayViewport = std::make_unique<rendercore::Viewport>(
				m_appSize.first,
				m_appSize.second,
				agl::ResourceFormat::R8G8B8A8_UNORM_SRGB,
				DefaultLogic::GetDefaultBackgroundColor() );
		}
		else
		{
			m_primayViewport = std::make_unique<rendercore::Viewport>( *m_canvas );
		}

		m_gameViewport = std::make_unique<GameClientViewport>( m_primayViewport.get() );
	}

	Owner<ILogic*> CreateGameLogic()
	{
		return new CGameLogic();
	}

	void DestroyGameLogic( Owner<ILogic*> pGameLogic )
	{
		delete pGameLogic;
	}
}
