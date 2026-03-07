#include "RenderGraph.h"

#include "CommandList.h"

#include <queue>
#include <stack>

namespace rendercore
{
	void RenderGraph::Execute()
	{
		CPU_PROFILE( RenderGraph_Execute );

		Compile();

		for ( auto pass : m_passes )
		{
			ExecutePassPrologue( *pass );

			bool isAsyncComputePass = ( pass->m_type == RenderGraphPassType::AsyncCompute );
			auto commandList = isAsyncComputePass ? GetComputeCommandList() : GetCommandList();
			pass->Execute( commandList );

			ExecutePassEpilogue( *pass );
			
			std::destroy_at( pass );
		}

		CleanUp();
	}

	void RenderGraph::Commit()
	{
		GetCommandList().Commit();
		GetComputeCommandList().Commit();
	}

	RenderGraphTexture* RenderGraph::RegisterExternalResource( agl::Texture* texture )
	{
		assert( texture != nullptr );

		auto found = m_externalTextures.find( texture );
		if ( found != std::end( m_externalTextures ) )
		{
			return found->second;
		}

		auto* rgTexture = m_allocator.Allocate<RenderGraphTexture>( 1 );
		std::construct_at( rgTexture );

		rgTexture->m_ref = texture;
		rgTexture->m_trait = texture->GetTrait();
		rgTexture->m_isExternal = true;
		rgTexture->m_isForUpload = HasAnyFlags( rgTexture->m_trait.m_access, agl::ResourceAccess::CpuWrite );

		m_textures.emplace_back( rgTexture );
		m_externalTextures.emplace( texture, rgTexture );

		return rgTexture;
	}

	RenderGraphBuffer* RenderGraph::RegisterExternalResource( agl::Buffer* buffer )
	{
		assert( buffer != nullptr );

		auto found = m_externalBuffers.find( buffer );
		if ( found != std::end( m_externalBuffers ) )
		{
			return found->second;
		}

		auto* rgBuffer = m_allocator.Allocate<RenderGraphBuffer>( 1 );
		std::construct_at( rgBuffer );

		rgBuffer->m_ref = buffer;
		rgBuffer->m_trait = buffer->GetTrait();
		rgBuffer->m_isExternal = true;
		rgBuffer->m_isForUpload = HasAnyFlags( rgBuffer->m_trait.m_access, agl::ResourceAccess::CpuWrite );

		m_buffers.emplace_back( rgBuffer );
		m_externalBuffers.emplace( buffer, rgBuffer );

		return rgBuffer;
	}

	agl::Texture* RenderGraph::ConvertToExternalResource( RenderGraphTexture* rgTexture )
	{
		assert( rgTexture != nullptr );

		if ( rgTexture->Get() == nullptr )
		{
			rgTexture->m_ref = GraphicsResourcePool::GetInstance().FindFreeTexture( rgTexture->GetTrait(), rgTexture->m_name );
		}

		if ( rgTexture->m_isExternal == false )
		{
			m_externalTextures.emplace( rgTexture->Get(), rgTexture );
		}

		rgTexture->m_isExternal = true;
		return rgTexture->Get();
	}

	agl::Buffer* RenderGraph::ConvertToExternalResource( RenderGraphBuffer* rgBuffer )
	{
		assert( rgBuffer != nullptr );

		if ( rgBuffer->Get() == nullptr )
		{
			rgBuffer->m_ref = GraphicsResourcePool::GetInstance().FindFreeBuffer( rgBuffer->GetTrait(), rgBuffer->m_name );
		}

		if ( rgBuffer->m_isExternal == false )
		{
			m_externalBuffers.emplace( rgBuffer->Get(), rgBuffer );
		}

		rgBuffer->m_isExternal = true;
		return rgBuffer->Get();
	}

	RenderGraphTexture* RenderGraph::CreateTexture( const agl::TextureTrait& trait, const char* name )
	{
		auto* rgTexture = m_allocator.Allocate<RenderGraphTexture>( 1 );
		std::construct_at( rgTexture );

		rgTexture->m_name = name;
		rgTexture->m_trait = trait;
		rgTexture->m_isForUpload = HasAnyFlags( trait.m_access, agl::ResourceAccess::CpuWrite );

		m_textures.emplace_back( rgTexture );

		return rgTexture;
	}

	RenderGraphBuffer* RenderGraph::CreateBuffer( const agl::BufferTrait& trait, const char* name )
	{
		auto* rgBuffer = m_allocator.Allocate<RenderGraphBuffer>( 1 );
		std::construct_at( rgBuffer );

		rgBuffer->m_name = name;
		rgBuffer->m_trait = trait;
		rgBuffer->m_isForUpload = HasAnyFlags( trait.m_access, agl::ResourceAccess::CpuWrite );

		m_buffers.emplace_back( rgBuffer );

		return rgBuffer;
	}

	void RenderGraph::PushGPUProfileEvent( GpuProfileData& gpuProfileData )
	{
		auto newEvent = m_allocator.Allocate<GpuProfileRenderGraphEvent>( 1 );
		std::construct_at( newEvent );

		newEvent->m_eventData = &gpuProfileData;
		newEvent->m_parent = m_curGPUProfileEvent;

		m_curGPUProfileEvent = newEvent;
	}

	void RenderGraph::PopGPUProfileEvent()
	{
		if ( m_curGPUProfileEvent )
		{
			m_curGPUProfileEvent = m_curGPUProfileEvent->m_parent;
		}
	}

	void RenderGraph::PushPipelineStateEvent( PipelineStatData& pipelineStatData )
	{
		auto newEvent = m_allocator.Allocate<PipelineStateRenderGraphEvent>( 1 );
		std::construct_at( newEvent );

		newEvent->m_eventData = &pipelineStatData;
		newEvent->m_parent = m_curPipelineStatEvent;

		m_curPipelineStatEvent = newEvent;
	}

	void RenderGraph::PopPipelineStateEvent()
	{
		m_curPipelineStatEvent = m_curPipelineStatEvent->m_parent;
	}

	void RenderGraph::Compile()
	{
		AdjacencyLists adjacencyLists = BuildPassAdjacencyLists();
		SortPasses( adjacencyLists );
		CompilePassEvent();
	}

	void RenderGraph::CleanUp()
	{
		m_externalTextures.clear();
		m_externalBuffers.clear();

		for ( RenderGraphTexture* texture : m_textures )
		{
			std::destroy_at( texture );
		}

		for ( RenderGraphBuffer* buffer : m_buffers )
		{
			std::destroy_at( buffer );
		}

		m_allocator.Purge();

		std::construct_at( &m_passes, m_allocator );
		std::construct_at( &m_externalTextures, m_allocator );
		std::construct_at( &m_externalBuffers, m_allocator );
		std::construct_at( &m_textures, m_allocator );
		std::construct_at( &m_buffers, m_allocator );

		m_curGPUProfileEvent = nullptr;
		m_curPipelineStatEvent = nullptr;
	}

	RenderGraph::AdjacencyLists RenderGraph::BuildPassAdjacencyLists()
	{
		AdjacencyLists adjancencyLists( m_passes.size(), IntegerVector( m_allocator ), m_allocator );

		for ( int32 i = 0; i < static_cast<int32>( m_passes.size() ); ++i )
		{
			RenderGraphPass& pass = *m_passes[i];

			for ( int32 j = i + 1; j < static_cast<int32>( m_passes.size() ); ++j )
			{
				RenderGraphPass& otherPass = *m_passes[j];

				for ( const auto& resourceSet : pass.m_resourceWrites )
				{
					if ( otherPass.m_resourceReads.contains( resourceSet ) )
					{
						adjancencyLists[i].push_back( j );
						break;
					}
				}
			}
		}

		return adjancencyLists;
	}

	void RenderGraph::SortPasses( const AdjacencyLists& adjacencyLists )
	{
		if ( m_passes.empty() )
		{
			return;
		}

		IntegerVector inDegree( m_passes.size(), 0, m_allocator );

		for ( int32 i = 0; i < static_cast<int32>( adjacencyLists.size() ); ++i )
		{
			for ( int32 j : adjacencyLists[i] )
			{
				++inDegree[j];
			}
		}

		using TopologySortPriorityQueue = std::priority_queue<int32, IntegerVector, std::greater<int32>>;
		std::greater<int32> greater;
		IntegerVector container( m_allocator );
		TopologySortPriorityQueue queue( greater, container );

		for ( int32 i = 0; i < static_cast<int32>( inDegree.size() ); ++i )
		{
			if ( inDegree[i] == 0 )
			{
				queue.push( i );
			}
		}

		RenderPassList sortedPasses( m_allocator );
		while ( queue.empty() == false )
		{
			int32 passIndex = queue.top();
			queue.pop();

			sortedPasses.emplace_back( m_passes[passIndex] );

			for ( int32 i : adjacencyLists[passIndex] )
			{
				--inDegree[i];
				if ( inDegree[i] == 0 )
				{
					queue.push( i );
				}

				bool waitForOtherQueue = GetQueueType( m_passes[passIndex]->m_type ) != GetQueueType( m_passes[i]->m_type );
				m_passes[i]->m_waitForOtherQueue |= waitForOtherQueue;
			}
		}

		m_passes = std::move( sortedPasses );
	}

	void RenderGraph::CompilePassEvent()
	{
		for ( auto pass : m_passes )
		{
			for ( GpuProfileRenderGraphEvent* cur = pass->m_gpuProfileEvent; cur != nullptr; cur = cur->m_parent )
			{
				if ( cur->m_firstPass == nullptr )
				{
					cur->m_firstPass = pass;
				}

				cur->m_lastPass = pass;
			}

			for ( PipelineStateRenderGraphEvent* cur = pass->m_pipelineStatEvent; cur != nullptr; cur = cur->m_parent )
			{
				if ( cur->m_firstPass == nullptr )
				{
					cur->m_firstPass = pass;
				}

				cur->m_lastPass = pass;
			}
		}
	}

	void RenderGraph::ExecutePassPrologue( RenderGraphPass& pass )
	{
		PreparePassResource( pass );

		pass.ApplyResourceBarrier();

		auto commandList = GetCommandList();
		BeginGpuProfileEvent( commandList, pass, pass.m_gpuProfileEvent );
		BeginPipelineStatEvent( commandList, pass, pass.m_pipelineStatEvent );
	}

	void RenderGraph::ExecutePassEpilogue( RenderGraphPass& pass )
	{
		auto commandList = GetCommandList();
		EndPipelineStatEvent( commandList, pass, pass.m_pipelineStatEvent );
		EndGpuProfileEvent( commandList, pass, pass.m_gpuProfileEvent );
	}

	void RenderGraph::PreparePassResource( RenderGraphPass& pass )
	{
		auto AllocatePassResource = []( RenderGraphResource* rgResource, bool isTexture )
			{
				if ( rgResource->m_isExternal )
				{
					return;
				}

				if ( isTexture )
				{
					auto* rgTexture = static_cast<RenderGraphTexture*>( rgResource );
					if ( rgTexture->m_ref.Get() != nullptr )
					{
						return;
					}

					rgTexture->m_ref = GraphicsResourcePool::GetInstance().FindFreeTexture( rgTexture->GetTrait(), rgTexture->m_name );
				}
				else
				{
					auto* rgBuffer = static_cast<RenderGraphBuffer*>( rgResource );
					if ( rgBuffer->m_ref.Get() != nullptr )
					{
						return;
					}

					rgBuffer->m_ref = GraphicsResourcePool::GetInstance().FindFreeBuffer( rgBuffer->GetTrait(), rgBuffer->m_name );
				}
			};

		for ( auto& passResource : pass.m_resourceReads )
		{
			bool isTexture = HasAnyFlags( passResource.m_flag, RenderGraphResourceFlag::Texture );
			AllocatePassResource( passResource.m_ptr, isTexture );
		}

		for ( auto& passResource : pass.m_resourceWrites )
		{
			bool isTexture = HasAnyFlags( passResource.m_flag, RenderGraphResourceFlag::Texture );
			AllocatePassResource( passResource.m_ptr, isTexture );
		}
	}

	agl::QueueType GetQueueType( RenderGraphPassType passType )
	{
		return passType == RenderGraphPassType::AsyncCompute ? agl::QueueType::Compute : agl::QueueType::Direct;
	}
}
