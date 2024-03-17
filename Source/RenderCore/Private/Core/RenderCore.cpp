#include "Renderer/IRenderCore.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "common.h"
#include "CommonRenderResource.h"
#include "Core/IEditor.h"
#include "CpuProfiler.h"
#include "ForwardRenderer.h"
#include "GlobalShaders.h"
#include "GpuProfiler.h"
#include "GraphicsResource/Canvas.h"
#include "GraphicsResource/Viewport.h"
#include "IAgl.h"
#include "RenderTargetPool.h"
#include "RenderView.h"
#include "Scene/Scene.h"
#include "ShaderCache.h"
#include "TaskScheduler.h"
#include "TransitionUtils.h"
#include "UserInterfaceRenderer.h"

#if _WIN64
#include <optional>
#include <shlobj.h>
#pragma comment(lib, "WinPixEventRuntime.lib")
#endif

namespace
{
#if _WIN64
	std::wstring GetLatestWinPixGpuCapturerPath()
	{
		LPWSTR programFilesPath = nullptr;
		SHGetKnownFolderPath( FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath );

		std::filesystem::path pixInstallationPath = programFilesPath;
		pixInstallationPath /= "Microsoft PIX";

		std::wstring newestVersionFound;

		for ( auto const& directory_entry : std::filesystem::directory_iterator( pixInstallationPath ) )
		{
			if ( directory_entry.is_directory() )
			{
				if ( newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str() )
				{
					newestVersionFound = directory_entry.path().filename().c_str();
				}
			}
		}

		if ( newestVersionFound.empty() )
		{
			return {};
		}

		return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
	}
#endif
}

namespace rendercore
{
	class RenderCore final : public IRenderCore
	{
	public:
		virtual bool BootUp() override;
		virtual bool IsReady() const override;

		virtual void HandleDeviceLost() override;
		virtual void AppSizeChanged() override;

		virtual IScene* CreateScene() override;
		virtual void RemoveScene( IScene* scene ) override;

		virtual void BeginRenderingViewGroup( RenderViewGroup& renderViewGroup ) override;

		virtual ~RenderCore() override;

	private:
		void Shutdown();
		Owner<SceneRenderer*> FindAndCreateSceneRenderer( const RenderViewGroup& renderViewGroup );

		mutable bool m_isReady = false;

		HMODULE m_hAgl = nullptr;
		agl::IAgl* m_agl = nullptr;

		std::map<ShadingMethod, SceneRenderer*> m_sceneRenderer;
		UserInterfaceRenderer* m_uiRenderer = nullptr;

#if _WIN64
		HMODULE m_hWinPixEventRuntime = nullptr;
		HMODULE m_hWinPixGpuCapturer = nullptr;
#endif
	};

	Owner<IRenderCore*> CreateRenderCore()
	{
		return new RenderCore();
	}

	void DestroyRenderCore( Owner<IRenderCore*> pRenderCore )
	{
		delete pRenderCore;
	}

	bool RenderCore::BootUp()
	{
#if _WIN64
		m_hWinPixEventRuntime = LoadLibrary( _T( "WinPixEventRuntime.dll" ) );

		std::optional<std::wstring> winPixGpuCapturerPath = GetLatestWinPixGpuCapturerPath();
		if ( winPixGpuCapturerPath )
		{
			m_hWinPixGpuCapturer = LoadLibrary( winPixGpuCapturerPath.value().c_str() );
		}
#endif

		m_hAgl = LoadModule( "Agl.dll" );
		if ( m_hAgl == nullptr )
		{
			return false;
		}

		m_agl = GetInterface<agl::IAgl>();
		if ( m_agl == nullptr )
		{
			return false;
		}

		if ( m_agl->BootUp() == false )
		{
			return false;
		}

		DefaultGraphicsResources::GetInstance().BootUp();

		m_uiRenderer = GetInterface<UserInterfaceRenderer>();
		if ( m_uiRenderer && m_uiRenderer->BootUp() == false )
		{
			return false;
		}

		GlobalShaders::GetInstance().BootUp();

		ShaderCache::LoadFromFile();

		GraphicsInterface().BootUp( m_agl );

		while ( IsReady() == false )
		{
			GetInterface<ITaskScheduler>()->ProcessThisThreadTask();
		}

		return true;
	}

	bool RenderCore::IsReady() const
	{
		if ( m_isReady == false )
		{
			m_isReady = GlobalShaders::GetInstance().IsReady()
				&& ShaderCache::IsLoaded();
		}

		return m_isReady;
	}

	void RenderCore::HandleDeviceLost()
	{
	}

	void RenderCore::AppSizeChanged()
	{
		m_agl->AppSizeChanged();
	}

	IScene* RenderCore::CreateScene()
	{
		return new Scene();
	}

	void RenderCore::RemoveScene( IScene* scene )
	{
		EnqueueRenderTask(
			[scene]()
			{
				delete scene;
			} );
	}

	void RenderCore::BeginRenderingViewGroup( RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( BeginRenderingViewGroup );

		auto commandList = GetCommandList();
		Canvas& canvas = renderViewGroup.GetCanvas();
		{
			CPU_PROFILE( RenderFrame );
			GPU_PROFILE( commandList, RenderFrame );

			const ColorF& bgColor = renderViewGroup.GetBackgroundColor();
			float clearColor[4] = { bgColor[0], bgColor[1], bgColor[2], bgColor[3] };

			canvas.OnBeginFrameRendering();
			canvas.Clear( clearColor );

			Viewport& viewport = renderViewGroup.GetViewport();
			viewport.Clear( clearColor );

			RenderTargetPool::GetInstance().Tick();

			SceneRenderer* pSceneRenderer = FindAndCreateSceneRenderer( renderViewGroup );
			if ( pSceneRenderer )
			{
				if ( pSceneRenderer->PreRender( renderViewGroup ) == false )
				{
					return;
				}

				pSceneRenderer->Render( renderViewGroup );
				pSceneRenderer->PostRender( renderViewGroup );
			}

			if ( m_uiRenderer )
			{
				m_uiRenderer->Render( renderViewGroup );
			}

			canvas.OnEndFrameRendering();

			if ( pSceneRenderer )
			{
				pSceneRenderer->WaitUntilRenderingIsFinish();
			}
		}

		{
			CPU_PROFILE( CommitRenderFrame );
			{
				CPU_PROFILE( Commit );
				commandList.Commit();
			}
			{
				CPU_PROFILE( Present );
				canvas.Present();
			}
		}

		GetGpuProfiler().GatherProfileData();
	}

	RenderCore::~RenderCore()
	{
		Shutdown();
	}

	void RenderCore::Shutdown()
	{
		DefaultGraphicsResources::GetInstance().Shutdown();

		for ( auto& kv : m_sceneRenderer )
		{
			EnqueueRenderTask( 
				[sceneRenderer = kv.second]()
				{
					delete sceneRenderer;
				} );
		}

		m_sceneRenderer.clear();

		ShaderCache::SaveToFile();
		ShaderCache::Shutdown();

		GlobalShaders::GetInstance().Shutdown();
		RenderTargetPool::GetInstance().Shutdown();

		GraphicsInterface().Shutdown();

		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>( [](){} );
		GetInterface<ITaskScheduler>()->Wait( handle );

		ShutdownModule( m_hAgl );

#if _WIN64
		FreeLibrary( m_hWinPixEventRuntime );
		FreeLibrary( m_hWinPixGpuCapturer );
#endif
	}

	Owner<SceneRenderer*> RenderCore::FindAndCreateSceneRenderer( const RenderViewGroup& renderViewGroup )
	{
		ShadingMethod shadingMethod = renderViewGroup.Scene().GetShadingMethod();

		auto& sceneRenderer = m_sceneRenderer[shadingMethod];
		if ( sceneRenderer == nullptr )
		{
			switch ( shadingMethod )
			{
			case ShadingMethod::Forward:
			{
				sceneRenderer = new ForwardRenderer;
				break;
			}
			default:
				break;
			}
		}

		return sceneRenderer;
	}
}
