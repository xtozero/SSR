#pragma once

#include "common.h"
#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "Math/Matrix3X3.h"
#include "OcclusionRendering.h"
#include "PassProcessor.h"
#include "SizedTypes.h"

#include <vector>
#include <map>

namespace rendercore
{
	class Canvas;
	class IScene;
	class Viewport;

	struct ForwardLightingResource;

	struct RenderViewShowFlags
	{
		bool m_bHitProxy = false;
	};

	struct RenderViewState
	{
		uint64 m_occlusionFrameCounter = 0;
		OcclusionQueryAllocator m_occlutionTestPool;
		std::map<uint32, OcclusionTestData> m_occlusionTestDataSet;
	};

	class RenderViewStateReference
	{
	public:
		RENDERCORE_DLL RenderViewState* Get() const;

		RENDERCORE_DLL void Allocate();

		RenderViewStateReference() = default;
		RENDERCORE_DLL ~RenderViewStateReference();

	private:
		RenderViewState* m_viewState = nullptr;
	};

	struct RenderView
	{
		// for view
		Vector m_viewOrigin = {};
		BasisVectorMatrix m_viewAxis = {};

		// for projection
		float m_nearPlaneDistance = 0.f;
		float m_farPlaneDistance = 0.f;
		float m_aspect = 0.f;
		float m_fov = 0.f;

		RenderViewState* m_state = nullptr;
	};

	struct RenderViewGroupInitializer final
	{
		IScene& m_scene;
		Canvas& m_cavas;
		Viewport& m_viewport;
		float m_elapsedTime = 0.f;
		float m_totalTime = 0.f;
		const RenderViewShowFlags* m_showFlags = nullptr;
	};

	class RenderViewGroup final
	{
	public:
		RENDERCORE_DLL RenderViewGroup( RenderViewGroupInitializer& initializer ) 
			: m_scene( initializer.m_scene )
			, m_canvas( initializer.m_cavas )
			, m_viewport( initializer.m_viewport )
			, m_elapsedTime( initializer.m_elapsedTime )
			, m_totalTime( initializer.m_totalTime )
		{
			if ( initializer.m_showFlags )
			{
				m_showFlags = *initializer.m_showFlags;
			}
		}

		RENDERCORE_DLL RenderView& AddRenderView();

		IScene& Scene() { return m_scene; }
		const IScene& Scene() const { return m_scene; }
		Canvas& GetCanvas() { return m_canvas; }
		const Canvas& GetCanvas() const { return m_canvas; }
		Viewport& GetViewport() { return m_viewport; }
		const Viewport& GetViewport() const { return m_viewport; }
		float GetElapsedTime() const { return m_elapsedTime; }
		float GetTotalTime() const { return m_totalTime; }
		RenderViewShowFlags& GetShowFlags() { return m_showFlags; }
		const RenderViewShowFlags& GetShowFlags() const { return m_showFlags; }

		const RenderView& GetRenderView( size_t index ) const
		{
			return m_views[index];
		}

		size_t NumRenderView() const
		{
			return m_views.size();
		}

	private:
		std::vector<RenderView> m_views;
		IScene& m_scene;
		Canvas& m_canvas;
		Viewport& m_viewport;
		float m_elapsedTime = 0.f;
		float m_totalTime = 0.f;
		RenderViewShowFlags m_showFlags;
	};
}
