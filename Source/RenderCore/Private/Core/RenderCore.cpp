#include "Renderer/IRenderCore.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "Core/IEditor.h"
#include "CpuProfiler/CpuProfiler.h"
#include "ForwardRenderer.h"
#include "GlobalShaders.h"
#include "GpuProfiler.h"
#include "GraphicsResource/Canvas.h"
#include "GraphicsResource/Viewport.h"
#include "GraphicsResourcePool.h"
#include "IAgl.h"
#include "LibraryTool/Common.h"
#include "PipelineStateCache.h"
#include "Platform/CommandLine.h"
#include "RenderGraph.h"
#include "RenderView.h"
#include "Scene/Scene.h"
#include "Shader.h"
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

		if ( std::filesystem::exists( pixInstallationPath ) == false )
		{
			return {};
		}

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
	class QueryLaneCountCS final : public GlobalShaderBase<ComputeShader, QueryLaneCountCS>
	{
		DEFINE_SHADER_PARAM( LaneCount );
	};

	REGISTER_GLOBAL_SHADER( QueryLaneCountCS, "Visibility/CS_QueryLaneCount.fx", "main" );

	class RenderCore final : public IRenderCore
	{
	public:
		virtual bool BootUp() override;
		virtual bool IsReady() const override;

		virtual void ReloadShaders() override;
		virtual void ReloadGlobalShaders() override;

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
		Owner<SceneRenderer*> FindOrCreateSceneRenderer( const RenderViewGroup& renderViewGroup );

		void QueryLaneCount() const;

		mutable bool m_isReady = false;

		HMODULE m_hAgl = nullptr;
		agl::IAgl* m_agl = nullptr;

		std::set<IScene*> m_scenes;

		RenderGraph m_renderGraph;

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
		if ( DefaultRenderCore::IsPIXEnabled() )
		{
			m_hWinPixEventRuntime = LoadLibrary( _T( "WinPixEventRuntime.dll" ) );

			std::optional<std::wstring> winPixGpuCapturerPath = GetLatestWinPixGpuCapturerPath();
			if ( winPixGpuCapturerPath )
			{
				m_hWinPixGpuCapturer = LoadLibrary( winPixGpuCapturerPath.value().c_str() );
			}
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

		QueryLaneCount();

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

	void RenderCore::ReloadShaders()
	{
		EnqueueRenderTask(
			[this]()
			{
				for ( IScene* scene : m_scenes )
				{
					scene->PreReloadShaders();
				}
			} );

		ShaderBase::ReloadShaders();

		EnqueueRenderTask(
			[this]()
			{
				for ( IScene* scene : m_scenes )
				{
					scene->PostReloadShaders();
				}
			});
	}

	void RenderCore::ReloadGlobalShaders()
	{
		m_isReady = false;
		GlobalShaders::GetInstance().Reload();
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
		auto scene = new Scene( world );
		m_scenes.emplace( scene );

		return scene;
	}

	void RenderCore::RemoveScene( IScene* scene )
	{
		m_scenes.erase( scene );

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

		GraphicsResourcePool::GetInstance().Tick();

		canvas.OnBeginFrameRendering();
		canvas.Clear();
	}

	void RenderCore::BeginRenderingViewGroup( RenderViewGroup& renderViewGroup )
	{
		CPU_PROFILE( BeginRenderingViewGroup );

		GetGpuProfiler().GatherProfileData();

		{
			CPU_PROFILE( RenderFrame );
			GPU_PROFILE_EVENT( m_renderGraph, RenderFrame );

			SceneRenderer* pSceneRenderer = FindOrCreateSceneRenderer( renderViewGroup );
			assert( pSceneRenderer != nullptr );

			pSceneRenderer->PreRender( m_renderGraph, renderViewGroup );

			if ( renderViewGroup.GetShowFlags().m_bHitProxy )
			{
				pSceneRenderer->RenderHitProxy( m_renderGraph, renderViewGroup );
			}
			else
			{
				pSceneRenderer->Render( m_renderGraph, renderViewGroup );
			}

			m_renderGraph.Execute();
		}
	}

	void RenderCore::EndFrameRendering( Canvas& canvas )
	{
		CPU_PROFILE( EndFrameRendering );

		if ( m_uiRenderer )
		{
			CPU_PROFILE( RenderUI );
			GPU_PROFILE_EVENT( m_renderGraph, RenderUI );

			m_uiRenderer->Render( m_renderGraph, canvas );
		}

		GetPrimitiveIdPool().DiscardAll();

		{
			CPU_PROFILE( Execute );
			m_renderGraph.Execute();
		}

		canvas.OnEndFrameRendering();

		{
			CPU_PROFILE( CommitRenderFrame );
			{
				CPU_PROFILE( Commit );
				RenderGraph::Commit();
			}
		}

		{
			CPU_PROFILE( Present );
			bool useVSync = DefaultRenderCore::UseVSync();
			bool allowTearing = DefaultRenderCore::AllowTearing();
			canvas.Present( useVSync, allowTearing );
		}
	}

	void RenderCore::GetRawHitProxyData( Viewport& viewport, std::vector<Color>& outHitProxyData )
	{
		HitProxyMap& hitProxyMap = viewport.GetHitPorxyMap();

		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>(
			[&hitProxyMap, &outHitProxyData]()
			{
				RenderGraph::Commit();

				GetInterface<agl::IAgl>()->WaitGPU();

				if ( hitProxyMap.CpuTexture() == nullptr )
				{
					return;
				}

				agl::LockedResource lockedResource = GraphicsInterface().Lock( hitProxyMap.CpuTexture(), agl::ResourceLockFlag::Read );

				const agl::TextureDesc& cpuTextureDesc = hitProxyMap.CpuTexture()->GetDesc();
				uint32 width = cpuTextureDesc.m_width;
				uint32 height = cpuTextureDesc.m_height;

				size_t rowSize = sizeof( Color ) * width;
				auto dest = outHitProxyData.data();
				auto src = static_cast<uint8*>( lockedResource.m_data );

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
		GraphicsResourcePool::GetInstance().Shutdown();

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

	Owner<SceneRenderer*> RenderCore::FindOrCreateSceneRenderer( const RenderViewGroup& renderViewGroup )
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

	void RenderCore::QueryLaneCount() const
	{
		if ( GetInterface<agl::IAgl>()->SupportsWaveIntrinsics() == false )
		{
			return;
		}

		if (engine::CommandLine::Has( StaticName( "AssetBuilder" ) ))
		{
			return;
		}

		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>(
			[]()
			{
				RenderGraph renderGraph;

				agl::BufferDesc laneCountDesc = {
					.m_stride = sizeof( uint32 ),
					.m_count = 1,
					.m_access = agl::ResourceAccess::Default,
					.m_bindType = agl::ResourceBindType::RandomAccess,
					.m_miscFlag = agl::ResourceMisc::BufferStructured,
					.m_format = agl::ResourceFormat::Unknown
				};

				RenderGraphBuffer* rgLaneCount = renderGraph.CreateBuffer( laneCountDesc, "LaneCount" );

				BEGIN_RG_RESOURCE_STRUCT( QueryLaneCountPassResource )
					DECLARE_RG_BUFFER_UAV( laneCount )
				END_RG_RESOURCE_STRUCT();

				QueryLaneCountPassResource queryPassResource = {
					.m_laneCount = rgLaneCount
				};

				renderGraph.AddPass(
					queryPassResource,
					[queryPassResource]( ComputeCommandList& commandList )
					{
						QueryLaneCountCS queryLaneCountCS;
						RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( queryLaneCountCS );

						commandList.BindPipelineState( pso.Get() );

						agl::ShaderBindings shaderBindings = CreateShaderBindings( queryLaneCountCS );
						BindResource( shaderBindings, queryLaneCountCS.LaneCount(), queryPassResource.m_laneCount->Get() );

						commandList.BindShaderResources( shaderBindings );

						commandList.Dispatch( 1, 1 );
					} );

				agl::BufferDesc readbackDesc = {
					.m_stride = sizeof( uint32 ),
					.m_count = 1,
					.m_access = agl::ResourceAccess::Download,
					.m_bindType = agl::ResourceBindType::None,
					.m_miscFlag = agl::ResourceMisc::None,
					.m_format = agl::ResourceFormat::Unknown
				};

				RefHandle<agl::Buffer> readBack = GraphicsResourcePool::GetInstance().FindFreeBuffer( readbackDesc, "Readback" );
				RenderGraphBuffer* rgReadback = renderGraph.RegisterExternalResource( readBack.Get() );

				BEGIN_RG_RESOURCE_STRUCT( DownloadPassResource )
					DECLARE_RG_BUFFER_COPY_SOURCE( laneCount )
					DECLARE_RG_BUFFER_COPY_DEST( readback )
				END_RG_RESOURCE_STRUCT();

				DownloadPassResource downloadPassResource = {
					.m_laneCount = rgLaneCount,
					.m_readback = rgReadback
				};

				renderGraph.AddPass(
					downloadPassResource,
					[downloadPassResource]( CopyCommandList& commandList )
					{
						commandList.CopyResource( downloadPassResource.m_readback->Get(), downloadPassResource.m_laneCount->Get(), false );
					} );

				renderGraph.Execute();

				RenderGraph::Commit();

				GetInterface<agl::IAgl>()->WaitGPU();

				NumLanes = *GraphicsInterface().Lock<uint32>( readBack.Get(), agl::ResourceLockFlag::Read );
				GraphicsInterface().UnLock( readBack.Get() );
			} );

		GetInterface<ITaskScheduler>()->Wait( handle );
	}

	uint32 NumLanes = 0xFFFFFFFF;
}
