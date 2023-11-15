#pragma once

#include "PassProcessor.h"
#include "SizedTypes.h"
#include "TypedBuffer.h"

namespace rendercore
{
	class IRendererRenderTargets;
	class RenderViewGroup;

	struct TAAResolveParameter final
	{
		float m_blendWeight;
		float padding[3];
	};

	class TAAResolveProcessor final : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};

	class TAARenderer final
	{
	public:
		void Render( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );
		void UploadParamToGpu();
		void Resovle( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );
		void UpdateHistory( IRendererRenderTargets& renderTargets, RenderViewGroup& renderViewGroup );

		TAARenderer();

	private:
		TypedConstatBuffer<TAAResolveParameter> m_resolveConstantBuffer;
		TAAResolveParameter m_resolveParam;
		bool m_paramUploaded = false;
	};
}