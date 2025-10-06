#include "RenderGraphPass.h"

#include "RenderGraph.h"
#include "RenderGraphResource.h"

namespace rendercore
{
	agl::ResourceState DetermineResourceState( RenderGraphResourceFlag type, bool isReadOnly )
	{
		if ( HasAnyFlags( type, RenderGraphResourceFlag::NonPixelSRV ) )
		{
			return agl::ResourceState::NonPixelShaderResource;
		}
		else if ( HasAnyFlags( type, RenderGraphResourceFlag::PixelSRV ) )
		{
			return agl::ResourceState::PixelShaderResource;
		}
		else if ( HasAnyFlags( type, RenderGraphResourceFlag::UAV ) )
		{
			return agl::ResourceState::UnorderedAccess;
		}
		else if ( HasAnyFlags( type, RenderGraphResourceFlag::RTV ) )
		{
			return agl::ResourceState::RenderTarget;
		}
		else if ( HasAnyFlags( type, RenderGraphResourceFlag::DSV ) )
		{
			return isReadOnly ? agl::ResourceState::DepthRead : agl::ResourceState::DepthWrite;
		}
		else if ( HasAnyFlags( type, RenderGraphResourceFlag::CopyDest ) )
		{
			return agl::ResourceState::CopyDest;
		}
		else if ( HasAnyFlags( type, RenderGraphResourceFlag::CopySource ) )
		{
			return agl::ResourceState::CopySource;
		}
		else
		{
			assert( false && "Unhandled render graph resource flag" );
		}

		return agl::ResourceState::Common;
	}

	template <typename ResourceType>
	void AddResourceBarrier( rendercore::ComputeCommandList& commandList, ResourceType& resource, agl::ResourceState afterState )
	{
		agl::ResourceState beforState = resource.GetResourceState();

		if ( beforState == agl::ResourceState::UnorderedAccess 
			&& afterState == agl::ResourceState::UnorderedAccess )
		{
			commandList.AddUavBarrier( UavBarrier( resource ) );
		}
		else
		{
			commandList.AddTransition( Transition( resource, afterState ) );
		}
	}

	void RenderGraphPass::ApplyResourceBarrier()
	{
		auto AddTransition = []( const RenderGraphResourceSet& resourceSet, bool isReadOnly )
		{
			auto commandList = GetCommandList();

			for ( const PassResource& resource : resourceSet )
			{
				agl::ResourceState resourceState = DetermineResourceState( resource.m_flag, isReadOnly );

				if ( HasAnyFlags( resource.m_flag, RenderGraphResourceFlag::Texture ) )
				{
					auto& texture = *static_cast<RenderGraphTexture*>(resource.m_ptr);
					AddResourceBarrier( commandList, *texture.Get(), resourceState );
				}
				else
				{
					auto& buffer = *static_cast<RenderGraphBuffer*>(resource.m_ptr);
					AddResourceBarrier( commandList, *buffer.Get(), resourceState );
				}
			}
		};

		auto AddFencePrologue = [this]()
		{
			if ( m_waitForOtherQueue == false )
			{
				return;
			}

			if ( GetQueueType( m_type ) == agl::QueueType::Direct )
			{
				GraphicsInterface().WaitQueue( agl::QueueType::Compute );
			}
		};

		auto AddFenceEpilogue = [this]()
		{
			if ( m_waitForOtherQueue == false )
			{
				return;
			}

			if ( GetQueueType( m_type ) == agl::QueueType::Compute )
			{
				GraphicsInterface().WaitQueue( agl::QueueType::Direct );
			}
		};

		AddFencePrologue();
		AddTransition( m_resourceReads, true );
		AddTransition( m_resourceWrites, false );
		AddFenceEpilogue();
	}

	void RenderGraphPass::LoadRasterOutput( CommandList& commandList )
	{
		if ( m_rasterOutput )
		{
			for ( auto& renderTarget : m_rasterOutput->m_renderTargets )
			{
				if ( renderTarget.m_texture && ( renderTarget.m_loadAction == RasterOutputLoadAction::Clear ) )
				{
					commandList.ClearRenderTarget( renderTarget.m_texture->Get()->RTV() );
				}
			}

			if ( m_rasterOutput->m_depthStencil.m_texture )
			{
				if ( m_rasterOutput->m_depthStencil.m_loadAction == RasterOutputLoadAction::Clear )
				{
					commandList.ClearDepthStencil( m_rasterOutput->m_depthStencil.m_texture->Get()->DSV() );
				}
			}
		}
	}

	void RenderGraphPass::Setup( void* resourceData, const RenderGraphResourceMemberMetaData* metaData, const RasterOutput* rasterOutput )
	{
		if ( resourceData )
		{
			for ( const RenderGraphResourceMemberMetaData* cur = metaData; cur != nullptr; cur = cur->m_next )
			{
				bool isSRV = HasAnyFlags( cur->m_flag, RenderGraphResourceFlag::SRV );
				bool isUAV = HasAnyFlags( cur->m_flag, RenderGraphResourceFlag::UAV );
				bool isCopyDest = HasAnyFlags( cur->m_flag, RenderGraphResourceFlag::CopyDest );
				bool isCopySorce = HasAnyFlags( cur->m_flag, RenderGraphResourceFlag::CopySource );

				auto resource = *reinterpret_cast<RenderGraphResource**>( static_cast<uint8*>( resourceData ) + cur->m_offset );

				if ( isSRV || isUAV || isCopySorce )
				{
					m_resourceReads.emplace( resource, cur->m_flag );
				}

				if ( isUAV || isCopyDest )
				{
					m_resourceWrites.emplace( resource, cur->m_flag );
				}
			}
		}

		m_rasterOutput = rasterOutput;
		if ( m_rasterOutput )
		{
			for ( auto& renderTarget : m_rasterOutput->m_renderTargets )
			{
				if ( auto renderTargetTex = renderTarget.m_texture )
				{
					m_resourceWrites.emplace( renderTargetTex, RenderGraphResourceFlag::Texture | RenderGraphResourceFlag::RTV );
				}
			}
			
			if ( auto depthStencilTex = m_rasterOutput->m_depthStencil.m_texture )
			{
				if ( m_rasterOutput->m_depthStencilReadOnly )
				{
					m_resourceReads.emplace( depthStencilTex, RenderGraphResourceFlag::Texture | RenderGraphResourceFlag::DSV );
				}
				else
				{
					m_resourceWrites.emplace( depthStencilTex, RenderGraphResourceFlag::Texture | RenderGraphResourceFlag::DSV );
				}
			}
		}
	}
}