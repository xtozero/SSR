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
#include "PipelineStateCache.h"
#include "RenderTargetPool.h"
#include "RenderView.h"
#include "ResourceBarrierUtils.h"
#include "Scene/Scene.h"
#include "ShaderCache.h"
#include "TaskScheduler.h"
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

		virtual IScene* CreateScene( logic::World& world ) override;
		virtual void RemoveScene( IScene* scene ) override;

		virtual void BeginFrameRendering( Canvas& canvas ) override;
		virtual void BeginRenderingViewGroup( RenderViewGroup& renderViewGroup ) override;
		virtual void EndFrameRendering( Canvas& canvas ) override;

		virtual void GetRawHitProxyData( Viewport& viewport, std::vector<Color>& outHitProxyData ) override;

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

		m_uiRenderer = GetInterface<UserInterfaceRenderer>();
		if ( m_uiRenderer && m_uiRenderer->BootUp() == false )
		{
			return false;
		}

		GlobalShaders::GetInstance().BootUp();

		ShaderCache::LoadFromFile();
		PipelineStateCache::LoadFromFile();

		GraphicsInterface().BootUp( m_agl );

		while ( !GlobalShaders::GetInstance().IsReady() || !ShaderCache::IsLoaded() )
		{
			GetInterface<ITaskScheduler>()->ProcessThisThreadTask();
		}

		DefaultGraphicsResources::GetInstance().BootUp();
		while ( !DefaultGraphicsResources::GetInstance().IsReady() )
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
				&& ShaderCache::IsLoaded()
				&& PipelineStateCache::IsLoaded()
				&& DefaultGraphicsResources::GetInstance().IsReady();
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

	IScene* RenderCore::CreateScene( logic::World& world )
	{
		return new Scene( world );
	}

	void RenderCore::RemoveScene( IScene* scene )
	{
		EnqueueRenderTask(
			[scene]()
			{
				delete scene;
			} );
	}

	void RenderCore::BeginFrameRendering( Canvas& canvas )
	{
		CPU_PROFILE( BeginFrameRendering );

		GetGpuProfiler().BeginFrameRendering();

		RenderTargetPool::GetInstance().Tick();

		canvas.OnBeginFrameRendering();
		canvas.Clear();
	}

	void RenderCore::BeginRenderingViewGroup( RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( BeginRenderingViewGroup );

		GetGpuProfiler().GatherProfileData();

		auto commandList = GetCommandList();
		{
			CPU_PROFILE( RenderFrame );
			GPU_PROFILE( commandList, RenderFrame );

			SceneRenderer* pSceneRenderer = FindAndCreateSceneRenderer( renderViewGroup );
			assert( pSceneRenderer != nullptr );

			pSceneRenderer->PreRender( renderViewGroup );

			if ( renderViewGroup.GetShowFlags().m_bHitProxy )
			{
				pSceneRenderer->RenderHitProxy( renderViewGroup );
			}
			else
			{
				pSceneRenderer->Render( renderViewGroup );
			}

			pSceneRenderer->PostRender();
			pSceneRenderer->WaitUntilRenderingIsFinish();
		}
	}

	void RenderCore::EndFrameRendering( Canvas& canvas )
	{
		CPU_PROFILE( EndFrameRendering );

		auto commandList = GetCommandList();
		{
			CPU_PROFILE( RenderUI );
			GPU_PROFILE( commandList, RenderUI );

			assert( m_uiRenderer != nullptr );
			m_uiRenderer->Render( canvas );
		}

		GetPrimitiveIdPool().DiscardAll();

		canvas.OnEndFrameRendering();

		{
			CPU_PROFILE( CommitRenderFrame );
			{
				CPU_PROFILE( Commit );
				commandList.Commit();
			}
		}

		{
			CPU_PROFILE( Present );
			canvas.Present();
		}
	}

	void RenderCore::GetRawHitProxyData( Viewport& viewport, std::vector<Color>& outHitProxyData )
	{
		HitProxyMap& hitProxyMap = viewport.GetHitPorxyMap();

		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>(
			[&hitProxyMap, &outHitProxyData]()
			{
				auto commandList = GetCommandList();
				commandList.CopyResource( hitProxyMap.CpuTexture(), hitProxyMap.Texture(), true );

				commandList.Commit();

				GetInterface<agl::IAgl>()->WaitGPU();

				if ( hitProxyMap.CpuTexture() == nullptr )
				{
					return;
				}

				agl::LockedResource lockedResource = GraphicsInterface().Lock( hitProxyMap.CpuTexture(), agl::ResourceLockFlag::Read );

				const agl::TextureTrait& cpuTextureTrait = hitProxyMap.CpuTexture()->GetTrait();
				uint32 width = cpuTextureTrait.m_width;
				uint32 height = cpuTextureTrait.m_height;

				size_t rowSize = sizeof( Color ) * width;
				auto dest = outHitProxyData.data();
				auto src = (uint8*)lockedResource.m_data;

				for ( uint32 i = 0; i < height; ++i )
				{
					std::memcpy( dest, src, rowSize );
					dest += width;
					src += lockedResource.m_rowPitch;
				}

				GraphicsInterface().UnLock( hitProxyMap.CpuTexture() );
			} );
		GetInterface<ITaskScheduler>()->Wait( handle );
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

		ShaderCache::Shutdown();

		GlobalShaders::GetInstance().Shutdown();
		RenderTargetPool::GetInstance().Shutdown();

		GraphicsInterface().Shutdown();

		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>( [](){} );
		GetInterface<ITaskScheduler>()->Wait( handle );

		PipelineStateCache::Shutdown();
		GetPrimitiveIdPool().Shutdown();

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
