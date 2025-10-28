#pragma once

#include "DrawSnapshot.h"

#include <optional>
#include <memory>

namespace rendercore
{
	enum class RenderPassType : uint8
	{
		DepthWrite = 0,
		CascadeShadowDepth,
		PointShadowDepth,
		Default,
		HitProxy,
		CompositeSSGI,
		Visibility,
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

	struct PassShader final
	{
		VertexShader* m_vertexShader = nullptr;
		GeometryShader* m_geometryShader = nullptr;
		PixelShader* m_pixelShader = nullptr;
		MeshShader* m_meshShader = nullptr;
		AmplificationShader* m_amplificationShader = nullptr;
	};

	struct PassRenderOption final
	{
		agl::ResourcePrimitive m_primitive = agl::ResourcePrimitive::Trianglelist;
		BlendOption* m_blendOption = nullptr;
		DepthStencilOption* m_depthStencilOption = nullptr;
		RasterizerOption* m_rasterizerOption = nullptr;
	};

	class IPassProcessor
	{
	public:
		std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh );

		virtual ~IPassProcessor() = default;

	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) = 0;
		virtual PassShader CollectPassShader( MaterialResource& material ) const;

		std::optional<DrawSnapshot> BuildDrawSnapshot( 
			const PrimitiveSubMesh& subMesh,
			const PassShader& passShader, 
			const PassRenderOption& passRenderOption, 
			VertexStreamLayoutType layoutType,
			bool useAutoInstancing = true );
	};

	using PassProcessorCreateFunction = IPassProcessor* (*)();

	class PassProcessorManager final
	{
	public:
		static IPassProcessor* GetPassProcessor( RenderPassType passType );

		static void RegisterCreateFunction( RenderPassType passType, PassProcessorCreateFunction createFunction );

	private:
		static std::unique_ptr<IPassProcessor> m_passProcessors[static_cast<uint32>( RenderPassType::Count )];
		static PassProcessorCreateFunction m_createFunctions[static_cast<uint32>( RenderPassType::Count )];
	};

	class PassProcessorRegister final
	{
	public:
		PassProcessorRegister( RenderPassType passType, PassProcessorCreateFunction createFunction );
	};
}
