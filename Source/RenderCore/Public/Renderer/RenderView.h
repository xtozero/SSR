#pragma once

#include "common.h"
#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "Math/Matrix3X3.h"
#include "PassProcessor.h"
#include "SizedTypes.h"

#include <vector>

namespace rendercore
{
	class IScene;
	class Viewport;

	struct ForwardLightingResource;

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

		VectorSingleFrame<VisibleDrawSnapshot>* m_snapshots;

		ForwardLightingResource* m_forwardLighting = nullptr;
	};

	struct RenderViewGroupInitializer
	{
		IScene& m_scene;
		Viewport& m_viewport;
		float m_elapsedTime = 0.f;
		float m_totalTime = 0.f;
	};

	class RenderViewGroup
	{
	public:
		RENDERCORE_DLL RenderViewGroup( RenderViewGroupInitializer& initializer ) 
			: m_scene( initializer.m_scene )
			, m_viewport( initializer.m_viewport )
			, m_elapsedTime( initializer.m_elapsedTime )
			, m_totalTime( initializer.m_totalTime )
		{}

		RENDERCORE_DLL RenderView& AddRenderView();

		size_t Size() const
		{
			return m_viewGroup.size();
		}

		RenderView* begin()
		{
			return m_viewGroup.size() > 0 ? &m_viewGroup[0] : nullptr;
		}

		RenderView* end()
		{
			RenderView* endIter = nullptr;
			if ( m_viewGroup.size() > 0 )
			{
				endIter = &m_viewGroup.back();
				++endIter;
			}

			return endIter;
		}

		IScene& Scene() { return m_scene; }
		const IScene& Scene() const { return m_scene; }
		Viewport& GetViewport() { return m_viewport; }
		const Viewport& GetViewport() const { return m_viewport; }
		float GetElapsedTime() const { return m_elapsedTime; }
		float GetTotalTime() const { return m_totalTime; }

		const RenderView& operator[]( size_t index ) const
		{
			return m_viewGroup[index];
		}

		RenderView& operator[]( size_t index )
		{
			return m_viewGroup[index];
		}

	private:
		std::vector<RenderView> m_viewGroup;
		IScene& m_scene;
		Viewport& m_viewport;
		float m_elapsedTime = 0.f;
		float m_totalTime = 0.f;
	};
}
