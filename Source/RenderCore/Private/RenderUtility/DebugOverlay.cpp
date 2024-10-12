#include "DebugOverlay.h"

#include "Config/DefaultRenderCoreConfig.h"
#include "SceneRenderer.h"
#include "VertexCollection.h"

#include <cassert>

namespace rendercore
{
	class DebugOverlayVS final : public GlobalShaderCommon<VertexShader, DebugOverlayVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class DebugOverlayPS final : public GlobalShaderCommon<PixelShader, DebugOverlayPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( DebugOverlayVS, "./Assets/Shaders/DebugOverlay/VS_DebugOverlay.asset" );
	REGISTER_GLOBAL_SHADER( DebugOverlayPS, "./Assets/Shaders/DebugOverlay/PS_DebugOverlay.asset" );

	void DebugOverlayData::Draw( GlobalDynamicVertexBuffer& dynamicVertexBuffer, RenderingShaderResource& resources )
	{
		auto numDebugLine = static_cast<uint32>( m_debugLine.size() );
		auto numDebugTriangle = static_cast<uint32>( m_debugTriangle.size() );

		if ( ( numDebugLine == 0 ) && ( numDebugTriangle == 0 ) )
		{
			return;
		}

		constexpr auto VertexSizeInBytes = static_cast<uint32>( sizeof( DebugOverlayVertex ) );
		auto lineVbAllocationInfo = dynamicVertexBuffer.Allocate( numDebugLine * VertexSizeInBytes );
		if ( lineVbAllocationInfo.m_lockedMemory != nullptr )
		{
			auto dest = static_cast<uint8*>( lineVbAllocationInfo.m_lockedMemory );
			std::memcpy( dest, m_debugLine.data(), numDebugLine * VertexSizeInBytes );
		}

		auto triangleVbAllocationInfo = dynamicVertexBuffer.Allocate( numDebugTriangle * VertexSizeInBytes );
		if ( triangleVbAllocationInfo.m_lockedMemory != nullptr )
		{
			auto dest = static_cast<uint8*>( triangleVbAllocationInfo.m_lockedMemory );
			std::memcpy( dest, m_debugTriangle.data(), numDebugTriangle * VertexSizeInBytes );
		}

		dynamicVertexBuffer.Commit();

		auto commandList = GetCommandList();

		StaticShaderSwitches switches = DebugOverlayVS::GetSwitches();
		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			switches.On( Name( "TAA" ), 1 );
		}

		DebugOverlayVS debugOverlayVS( switches );
		DebugOverlayPS debugOverlayPS;

		VertexStreamLayout vertexlayout;
		vertexlayout.AddLayout( "POSITION", 0, agl::ResourceFormat::R32G32B32_FLOAT, 0, false, 0, 0 );
		vertexlayout.AddLayout( "COLOR", 0, agl::ResourceFormat::R32G32B32A32_FLOAT, 0, false, 0, 0 );

		VertexLayout vertexLayout = GraphicsInterface().FindOrCreate( *debugOverlayVS, vertexlayout );

		DepthStencilState debugOverlayDepthState = StaticDepthStencilState<true, false, agl::ComparisonFunc::LessEqual>::Get();
		RasterizerState debugOverlayRasterizerState = StaticRasterizerState<>::Get();

		if ( lineVbAllocationInfo.m_lockedMemory != nullptr )
		{
			DrawSnapshot snapshot;
			snapshot.m_vertexStream.Bind( lineVbAllocationInfo.m_buffer, 0, VertexSizeInBytes, lineVbAllocationInfo.m_offset );
			snapshot.m_primitiveIdSlot = -1;

			snapshot.m_pipelineState.m_shaderState.m_vertexLayout = vertexLayout;
			snapshot.m_pipelineState.m_shaderState.m_vertexShader = debugOverlayVS;
			snapshot.m_pipelineState.m_shaderState.m_pixelShader = debugOverlayPS;

			snapshot.m_pipelineState.m_rasterizerState = debugOverlayRasterizerState;
			snapshot.m_pipelineState.m_depthStencilState = debugOverlayDepthState;

			snapshot.m_pipelineState.m_primitive = agl::ResourcePrimitive::Linelist;

			auto initializer = CreateShaderBindingsInitializer( snapshot.m_pipelineState.m_shaderState );
			snapshot.m_shaderBindings.Initialize( initializer );

			snapshot.m_count = numDebugLine;

			PreparePipelineStateObject( snapshot );

			resources.BindResources( snapshot.m_pipelineState.m_shaderState, snapshot.m_shaderBindings );

			VisibleDrawSnapshot visibleSnapshot = {
				.m_primitiveId = 0,
				.m_primitiveIdOffset = 0,
				.m_numInstance = 1,
				.m_snapshotBucketId = -1,
				.m_drawSnapshot = &snapshot,
			};

			VertexBuffer emptyPrimitiveID;
			CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
		}

		if ( triangleVbAllocationInfo.m_lockedMemory != nullptr )
		{
			DrawSnapshot snapshot;
			snapshot.m_vertexStream.Bind( triangleVbAllocationInfo.m_buffer, 0, VertexSizeInBytes, triangleVbAllocationInfo.m_offset );
			snapshot.m_primitiveIdSlot = -1;

			snapshot.m_pipelineState.m_shaderState.m_vertexLayout = vertexLayout;
			snapshot.m_pipelineState.m_shaderState.m_vertexShader = debugOverlayVS;
			snapshot.m_pipelineState.m_shaderState.m_pixelShader = debugOverlayPS;

			snapshot.m_pipelineState.m_rasterizerState = debugOverlayRasterizerState;
			snapshot.m_pipelineState.m_depthStencilState = debugOverlayDepthState;

			snapshot.m_pipelineState.m_primitive = agl::ResourcePrimitive::Trianglelist;

			auto initializer = CreateShaderBindingsInitializer( snapshot.m_pipelineState.m_shaderState );
			snapshot.m_shaderBindings.Initialize( initializer );

			snapshot.m_count = numDebugTriangle;

			PreparePipelineStateObject( snapshot );

			resources.BindResources( snapshot.m_pipelineState.m_shaderState, snapshot.m_shaderBindings );

			VisibleDrawSnapshot visibleSnapshot = {
				.m_primitiveId = 0,
				.m_primitiveIdOffset = 0,
				.m_numInstance = 1,
				.m_snapshotBucketId = -1,
				.m_drawSnapshot = &snapshot,
			};

			VertexBuffer emptyPrimitiveID;
			CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );
		}
	}

	void DebugOverlayData::AddLine( const Point& from, const Point& to, const ColorF& color )
	{
		m_debugLine.emplace_back( from, color );
		m_debugLine.emplace_back( to, color );
	}

	void DebugOverlayData::AddTriangle( const Point& p0, const Point& p1, const Point& p2, const ColorF& color )
	{
		m_debugTriangle.emplace_back( p0, color );
		m_debugTriangle.emplace_back( p1, color );
		m_debugTriangle.emplace_back( p2, color );
	}

	DebugOverlayVertex::DebugOverlayVertex( const Vector& position, const ColorF& color )
		: m_position( position )
		, m_color( color )
	{}
}
