#pragma once

#include "DrawSnapshot.h"

#include <optional>
#include <memory>

namespace rendercore
{
	enum class RenderPass : uint8
	{
		DepthWrite = 0,
		CSMShadowDepth,
		Default,
		Count,
	};

	class BlendOption;
	class DepthStencilOption;
	class GeometryShader;
	class MaterialResource;
	class PixelShader;
	class PrimitiveProxy;
	class RasterizerOption;
	class VertexShader;

	enum class VertexStreamLayoutType : uint8;

	struct PrimitiveSubMesh;

	struct PassShader
	{
		VertexShader* m_vertexShader = nullptr;
		GeometryShader* m_geometryShader = nullptr;
		PixelShader* m_pixelShader = nullptr;
	};

	struct PassRenderOption
	{
		BlendOption* m_blendOption = nullptr;
		DepthStencilOption* m_depthStencilOption = nullptr;
		RasterizerOption* m_rasterizerOption = nullptr;
	};

	class IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) = 0;

		virtual ~IPassProcessor() = default;

	protected:
		virtual PassShader CollectPassShader( MaterialResource& material ) const;

		std::optional<DrawSnapshot> BuildDrawSnapshot( const PrimitiveSubMesh& subMesh, const PassShader& passShader, const PassRenderOption& passRenderOption, VertexStreamLayoutType layoutType );
	};

	using PassProcessorCreateFunction = IPassProcessor * ( * )( );

	class PassProcessorManager
	{
	public:
		static IPassProcessor* GetPassProcessor( RenderPass passType );

		static void RegisterCreateFunction( RenderPass passType, PassProcessorCreateFunction createFunction );

	private:
		static std::unique_ptr<IPassProcessor> m_passProcessors[static_cast<uint32>( RenderPass::Count )];
		static PassProcessorCreateFunction m_createFunctions[static_cast<uint32>( RenderPass::Count )];
	};

	class PassProcessorRegister
	{
	public:
		PassProcessorRegister( RenderPass passType, PassProcessorCreateFunction createFunction );
	};
}
