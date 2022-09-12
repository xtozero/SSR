#include "stdafx.h"
#include "Renderer/IRenderCore.h"

#include "AbstractGraphicsInterface.h"
#include "common.h"
#include "DefaultConstantBuffers.h"
#include "ForwardRenderer.h"
#include "GlobalShaders.h"
#include "IAga.h"
#include "RenderView.h"
#include "TaskScheduler.h"
#include "Scene/Scene.h"
#include "ShaderCache.h"

namespace rendercore
{
	class RenderCore : public IRenderCore
	{
	public:
		virtual bool BootUp() override;
		virtual bool IsReady() const override;

		virtual void HandleDeviceLost() override;
		virtual void AppSizeChanged() override;

		virtual IScene* CreateScene() override;
		virtual void RemoveScene( IScene* scene ) override;

		virtual void BeginRenderingViewGroup( RenderViewGroup& renderViewGroup ) override;

		~RenderCore();

	private:
		void Shutdown();
		SceneRenderer* FindAndCreateSceneRenderer( const RenderViewGroup& renderViewGroup );

		mutable bool m_isReady = false;

		HMODULE m_hAga;
		aga::IAga* m_aga = nullptr;

		std::map<SHADING_METHOD, std::unique_ptr<SceneRenderer>> m_sceneRenderer;
	};

	Owner<IRenderCore*> CreateRenderCore()
	{
		return new RenderCore();
	}

	void DestoryRenderCore( Owner<IRenderCore*> pRenderCore )
	{
		delete pRenderCore;
	}

	bool RenderCore::BootUp()
	{
		m_hAga = LoadModule( "Aga.dll" );
		if ( m_hAga == nullptr )
		{
			return false;
		}

		m_aga = GetInterface<aga::IAga>();
		if ( m_aga == nullptr )
		{
			return false;
		}

		if ( m_aga->BootUp() == false )
		{
			return false;
		}

		GlobalShader::GetInstance().BootUp();

		ShaderCache::LoadFromFile();

		GraphicsInterface().BootUp( m_aga );

		DefaultConstantBuffers::GetInstance().BootUp();

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
			m_isReady = GlobalShader::GetInstance().IsReady()
				&& ShaderCache::IsLoaded();
		}

		return m_isReady;
	}

	void RenderCore::HandleDeviceLost()
	{
	}

	void RenderCore::AppSizeChanged()
	{
		m_aga->AppSizeChanged();
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

		SceneRenderer::WaitUntilRenderingIsFinish();
	}

	RenderCore::~RenderCore()
	{
		Shutdown();
	}

	void RenderCore::Shutdown()
	{
		m_sceneRenderer.clear();

		ShaderCache::SaveToFile();

		GlobalShader::GetInstance().Shutdown();

		DefaultConstantBuffers::GetInstance().Shutdown();

		GraphicsInterface().Shutdown();

		ShutdownModule( m_hAga );
	}

	SceneRenderer* RenderCore::FindAndCreateSceneRenderer( const RenderViewGroup& renderViewGroup )
	{
		SHADING_METHOD shadingMethod = renderViewGroup.Scene().ShadingMethod();

		auto& sceneRenderer = m_sceneRenderer[shadingMethod];
		if ( sceneRenderer.get() == nullptr )
		{
			switch ( shadingMethod )
			{
			case SHADING_METHOD::Forward:
			{
				sceneRenderer = std::make_unique<ForwardRenderer>();
				break;
			}
			default:
				break;
			}
		}

		return sceneRenderer.get();
	}
}
