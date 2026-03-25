#pragma once

#include "CommandList.h"
#include "RenderGraphPass.h"
#include "RenderGraphResource.h"
#include "StackMemoryAllocator.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace agl
{
	class Buffer;
	class Texture;
}

namespace rendercore
{
	template <typename T>
	concept ComputePass = std::is_same_v<typename LambdaRenderGraphPass<T>::CommandListType, ComputeCommandList>;

	class RenderGraph
	{
	public:
		template <typename Lambda>
		void AddPass( Lambda&& passBody )
		{
			static_assert( !std::is_same_v<typename LambdaRenderGraphPass<Lambda>::CommandListType, CommandList>, "Graphics command list must need raster output" );

			constexpr RenderGraphPassType renderGraphPassType = ComputePass<Lambda> ? RenderGraphPassType::Compute : RenderGraphPassType::Copy;
			AddPassInternal( nullptr, nullptr, nullptr, renderGraphPassType, std::forward<Lambda>( passBody ) );
		}

		template <HasMetaData RenderGraphResource, typename Lambda>
		void AddPass( RenderGraphResource& renderGraphResource, Lambda&& passBody )
		{
			static_assert( !std::is_same_v<typename LambdaRenderGraphPass<Lambda>::CommandListType, CommandList>, "Graphics command list must need raster output" );

			constexpr RenderGraphPassType renderGraphPassType = ComputePass<Lambda> ? RenderGraphPassType::Compute : RenderGraphPassType::Copy;
			AddPassInternal( &renderGraphResource, renderGraphResource.GetMetaData(), nullptr, renderGraphPassType, std::forward<Lambda>( passBody ) );
		}

		template <ComputePass Lambda>
		void AddPass( bool isAsync, Lambda&& passBody )
		{
			static_assert( !std::is_same_v<typename LambdaRenderGraphPass<Lambda>::CommandListType, CommandList>, "Graphics command list must need raster output" );

			RenderGraphPassType renderGraphPassType = isAsync ? RenderGraphPassType::AsyncCompute : RenderGraphPassType::Compute;
			AddPassInternal( nullptr, nullptr, nullptr, renderGraphPassType, std::forward<Lambda>( passBody ) );
		}

		template <HasMetaData RenderGraphResource, ComputePass Lambda>
		void AddPass( bool isAsync, RenderGraphResource& renderGraphResource, Lambda&& passBody )
		{
			static_assert( !std::is_same_v<typename LambdaRenderGraphPass<Lambda>::CommandListType, CommandList>, "Graphics command list must need raster output" );

			RenderGraphPassType renderGraphPassType = isAsync ? RenderGraphPassType::AsyncCompute : RenderGraphPassType::Compute;
			AddPassInternal( &renderGraphResource, renderGraphResource.GetMetaData(), nullptr, renderGraphPassType, std::forward<Lambda>( passBody ) );
		}

		template <typename Lambda>
		void AddPass( const RasterOutput& rasterOutput, Lambda&& passBody )
		{
			static_assert( std::is_same_v<typename LambdaRenderGraphPass<Lambda>::CommandListType, CommandList>, "compute command list can't set raster output" );

			AddPassInternal( nullptr, nullptr, &rasterOutput, RenderGraphPassType::Graphics, std::forward<Lambda>( passBody ) );
		}

		template <HasMetaData RenderGraphResource, typename Lambda>
		void AddPass( RenderGraphResource& renderGraphResource, const RasterOutput& rasterOutput, Lambda&& passBody )
		{
			static_assert( std::is_same_v<typename LambdaRenderGraphPass<Lambda>::CommandListType, CommandList>, "compute command list can't set raster output" );

			AddPassInternal( &renderGraphResource, renderGraphResource.GetMetaData(), &rasterOutput, RenderGraphPassType::Graphics, std::forward<Lambda>( passBody ) );
		}

		template <typename Lambda>
		void AddPassInternal( void* resourceData, const RenderGraphResourceMemberMetaData* metaData, const RasterOutput* rasterOutput, RenderGraphPassType passType, Lambda&& passBody )
		{
			RasterOutput* passRasterOutput = nullptr;
			if ( rasterOutput )
			{
				passRasterOutput = m_allocator.Allocate<RasterOutput>( 1 );
				std::construct_at( passRasterOutput, *rasterOutput );
			}

			auto renderPass = m_allocator.Allocate<LambdaRenderGraphPass<Lambda>>( 1 );
			std::construct_at( renderPass, std::forward<Lambda>( passBody ) );

			renderPass->m_gpuProfileEvent = m_curGPUProfileEvent;
			renderPass->m_pipelineStatEvent = m_curPipelineStatEvent;
			renderPass->m_type = passType;

			renderPass->Setup( resourceData, metaData, passRasterOutput );

			m_passes.emplace_back( renderPass );
		}

		void Execute();
		static void Commit();

		RenderGraphTexture* RegisterExternalResource( agl::Texture* texture );
		RenderGraphBuffer* RegisterExternalResource( agl::Buffer* buffer );

		agl::Texture* ConvertToExternalResource( RenderGraphTexture* rgTexture );
		agl::Buffer* ConvertToExternalResource( RenderGraphBuffer* rgBuffer );

		RenderGraphTexture* CreateTexture( const agl::TextureTrait& trait, const char* name );
		RenderGraphBuffer* CreateBuffer( const agl::BufferTrait& trait, const char* name );

		void PushGPUProfileEvent( GpuProfileData& gpuProfileData );
		void PopGPUProfileEvent();

		void PushPipelineStateEvent( PipelineStatData& pipelineStatData );
		void PopPipelineStateEvent();

	private:
		void Compile();
		void CleanUp();

		using IntegerVector = std::vector<int32, StackAllocatorAdapter<int32>>;
		using AdjacencyLists = std::vector<IntegerVector, StackAllocatorAdapter<IntegerVector>>;

		AdjacencyLists BuildPassAdjacencyLists();

		using RenderPassList = std::vector<RenderGraphPass*, StackAllocatorAdapter<RenderGraphPass*>>;
		void SortPasses( const AdjacencyLists& adjacencyLists );

		void CompilePassEvent();

		void ExecutePassPrologue( RenderGraphPass& pass );
		void ExecutePassEpilogue( RenderGraphPass& pass );

		void PreparePassResource( RenderGraphPass& pass );

		StackAllocator m_allocator;

		RenderPassList m_passes{ m_allocator };

		using ExternalTextures = std::map<agl::Texture*, RenderGraphTexture*, std::less<agl::Texture*>, StackAllocatorAdapter<std::pair<agl::Texture* const, RenderGraphTexture*>>>;
		ExternalTextures m_externalTextures{ m_allocator };

		using ExternalBuffers = std::map<agl::Buffer*, RenderGraphBuffer*, std::less<agl::Buffer*>, StackAllocatorAdapter<std::pair<agl::Buffer* const, RenderGraphBuffer*>>>;
		ExternalBuffers m_externalBuffers{ m_allocator };

		std::vector<RenderGraphTexture*, StackAllocatorAdapter<RenderGraphTexture*>> m_textures{ m_allocator };
		std::vector<RenderGraphBuffer*, StackAllocatorAdapter<RenderGraphBuffer*>> m_buffers{ m_allocator };

		GpuProfileRenderGraphEvent* m_curGPUProfileEvent = nullptr;
		PipelineStateRenderGraphEvent* m_curPipelineStatEvent = nullptr;
	};

	agl::QueueType GetQueueType( RenderGraphPassType passType );
}