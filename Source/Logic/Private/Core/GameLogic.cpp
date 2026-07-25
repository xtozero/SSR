#include "Core/GameLogic.h"

#include "Config/DefaultAglConfig.h"
#include "Config/DefaultAppConfig.h"
#include "Config/DefaultLogicConfig.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "Core/IEngine.h"
#include "Core/Timer.h"
#include "Core/UtilWindowInfo.h"
#include "CpuProfiler/CpuProfiler.h"
#include "DebugUtil.h"
#include "FileSystem.h"
#include "GameObject/GameObject.h"
#include "IAgl.h"
#include "InterfaceFactories.h"
#include "Json/json.hpp"
#include "LibraryTool/Common.h"
#include "Platform/CommandLine.h"
#include "Platform/IPlatform.h"
#include "Renderer/IRenderCore.h"
#include "Renderer/RenderView.h"
#include "Scene/GameClientViewport.h"
#include "Scene/IScene.h"
#include "UserInput/UserInput.h"
#include "UserInterfaceRenderer.h"
#include "World/WorldLoader.h"

#include <ctime>

namespace
{
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
	bool GameLogic::BootUp( engine::IPlatform& platform )
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

		m_windowCtx.m_nativeContext = platform.GetRawInstance<HINSTANCE>();
		m_windowCtx.m_nativeWindow = platform.GetRawHandle<HWND>();
		srand( static_cast<uint32>( time( nullptr ) ) );

		m_appSize = platform.GetSize();
		UtilWindowInfo::GetInstance().SetRect( m_appSize.first, m_appSize.second );

		if ( m_pRenderCore->BootUp( m_windowCtx ) == false )
		{
			return false;
		}

		bool useGUI = engine::CommandLine::Has( StaticName( "Console" ) ) == false;

		if ( useGUI )
		{
			CreateGameViewport();
		}

		m_inputController = std::make_unique<PlayerController>();

		if ( useGUI && ( LoadWorld( DefaultLogic::GetDefaultWorld() ) == false ) )
		{
			__debugbreak();
		}

		m_world.Initialize( *this );

		return true;
	}

	void GameLogic::Update()
	{
		CPU_PROFILE( Update );

		// Update ElapsedTime at the start of a frame
		StartLogic();
		ProcessLogic();
		EndLogic();
	}

	void GameLogic::Pause()
	{
		m_world.Pause();
	}

	void GameLogic::Resume()
	{
		m_world.Resume();
	}

	bool GameLogic::HandleUserInput( [[maybe_unused]] engine::IPlatform& platform, const engine::UserInput& input )
	{
		if ( m_inputController == nullptr )
		{
			return false;
		}

		m_inputController->ProcessInput( input );
		return true;
	}

	void GameLogic::HandleTextInput( [[maybe_unused]] uint64 text, [[maybe_unused]] bool bUnicode )
	{
	}

	void GameLogic::AppSizeChanged( engine::IPlatform& platform )
	{
		const std::pair<uint32, uint32>& newAppSize = platform.GetSize();

		if ( m_appSize == newAppSize )
		{
			return;
		}

		m_appSize = newAppSize;
		UtilWindowInfo::GetInstance().SetRect( m_appSize.first, m_appSize.second );

		if ( m_canvas->Handle() == platform.GetRawHandle<void*>() )
		{
			m_canvas->Resize( m_appSize.first, m_appSize.second );
		}
	}

	GameClientViewport* GameLogic::GetGameClientViewport()
	{
		return m_gameViewport.get();
	}

	bool GameLogic::LoadWorld( const char* filePath )
	{
		UnloadWorld();

		IFileSystem* fileSystem = GetInterface<IFileSystem>();
		FileHandle worldAsset = fileSystem->OpenFile( filePath );

		if ( worldAsset.IsValid() == false )
		{
			return false;
		}

		unsigned long fileSize = fileSystem->GetFileSize( worldAsset );
		auto buffer = new char[fileSize];

		IFileSystem::IOCompletionCallback ParseWorldAsset;
		ParseWorldAsset.BindFunctor(
			[this, worldAsset]( const char* buffer, unsigned long bufferSize )
			{
				WorldLoader::Load( *this, buffer, static_cast<size_t>( bufferSize ) );
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

	void GameLogic::UnloadWorld()
	{
		for ( auto& object : m_world.GameObjects() )
		{
			RemoveObject( *object );
		}
	}

	World& GameLogic::GetWorld()
	{
		return m_world;
	}

	void GameLogic::SpawnObject( Owner<GameObject*> object )
	{
		m_world.SpawnObject( *this, object );
	}

	InputController* GameLogic::GetInputController()
	{
		return m_inputController.get();
	}

	void GameLogic::Shutdown()
	{
		WaitRenderThreadForShutdown();

		m_world.CleanUp();
		m_canvas.reset();
		m_primayViewport.reset();

		ShutdownModule( m_renderCoreDll );
	}

	void GameLogic::StartLogic()
	{
		CPU_PROFILE( StartLogic );

		// Preprocessing before executing game logic
		GetInterface<engine::IConsoleMessageExecutor>()->Execute();
		GetInterface<engine::IEngine>()->ProcessInput();

		m_world.BeginFrame();
	}

	void GameLogic::ProcessLogic()
	{
		CPU_PROFILE( ProcessLogic );

		if ( m_world.GetTimer().IsPaused() == false )
		{
			// Perform game logic
			m_world.RunFrame();
		}
	}

	void GameLogic::EndLogic()
	{
		CPU_PROFILE( EndLogic );

		// Reflect the results of the physics simulation
		m_world.EndFrame();

		GetTransientAllocator<ThreadType::GameThread>().Purge();

		// Post-processing after game logic execution
		if ( m_numDrawRequestQueued.load( std::memory_order_relaxed ) >= agl::DefaultAgl::GetBufferCount() )
		{
			return;
		}

		m_numDrawRequestQueued.fetch_add( 1, std::memory_order_release );
		UpdateUIDrawInfo();
		DrawScene();

		EnqueueRenderTask(
			[this]()
			{
				m_numDrawRequestQueued.fetch_add( -1, std::memory_order_release );
			} );
	}

	void GameLogic::DrawScene()
	{
		rendercore::IScene* scene = m_world.Scene();
		rendercore::Viewport* viewport = m_gameViewport->GetViewport();
		auto renderModule = GetInterface<rendercore::IRenderCore>();

		if ( ( scene == nullptr )
			|| ( viewport == nullptr )
			|| ( renderModule == nullptr )
			|| ( renderModule->IsReady() == false ) )
		{
			return;
		}

		EnqueueRenderTask(
			[scene]()
			{
				scene->OnBeginSceneRendering();
			} );

		m_gameViewport->BeginFrameRendering( *m_canvas );
		m_gameViewport->Draw( *m_canvas );
		m_gameViewport->EndFrameRendering( *m_canvas );
	}

	void GameLogic::UpdateUIDrawInfo()
	{
		auto uiRenderer = GetInterface<rendercore::UserInterfaceRenderer>();
		if ( uiRenderer )
		{
			uiRenderer->UpdateUIDrawInfo( *m_canvas );
		}
	}

	GameLogic::~GameLogic()
	{
		Shutdown();
	}

	void GameLogic::CreateGameViewport()
	{
		m_canvas = std::make_unique<rendercore::Canvas>(
			m_appSize.first,
			m_appSize.second,
			m_windowCtx,
			agl::ResourceFormat::R8G8B8A8_UNORM,
			rendercore::DefaultRenderCore::GetDefaultBackgroundColor() );

		if ( engine::DefaultApp::IsEditor() )
		{
			m_primayViewport = std::make_unique<rendercore::Viewport>(
				m_appSize.first,
				m_appSize.second,
				agl::ResourceFormat::R8G8B8A8_UNORM,
				rendercore::DefaultRenderCore::GetDefaultBackgroundColor() );
		}
		else
		{
			m_primayViewport = std::make_unique<rendercore::Viewport>( *m_canvas );
		}

		m_gameViewport = std::make_unique<GameClientViewport>( m_world, m_primayViewport.get() );
	}

	Owner<ILogic*> CreateGameLogic()
	{
		return new GameLogic();
	}

	void DestroyGameLogic( Owner<ILogic*> pGameLogic )
	{
		delete pGameLogic;
	}
}
