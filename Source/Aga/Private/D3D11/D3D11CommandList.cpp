#include "stdafx.h"
#include "D3D11CommandList.h"

#include "D3D11Api.h"
#include "D3D11BaseTexture.h"
#include "D3D11Buffer.h"
#include "D3D11PipelineState.h"
#include "D3D11ResourceViews.h"
#include "D3D11SamplerState.h"

#include "ShaderBindings.h"

#include <cassert>
#include <d3d11.h>

namespace aga
{
	class D3D11CommandList
	{
	public:
		static void BindVertexBuffer( ID3D11DeviceContext& context, Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
		{
			ID3D11Buffer* pBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
			uint32 strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
			uint32 offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

			if ( vertexBuffers )
			{
				assert( pOffsets != nullptr );
				for ( uint32 i = 0; i < numBuffers; ++i )
				{
					if ( auto d3d11buffer = static_cast<D3D11Buffer*>( vertexBuffers[i] ) )
					{
						pBuffers[i] = d3d11buffer->Resource( );
						strides[i] = d3d11buffer->Stride( );
						offsets[i] = pOffsets[i];
					}
				}
			}

			context.IASetVertexBuffers( startSlot, numBuffers, pBuffers, strides, offsets );
		}

		static void BindIndexBuffer( ID3D11DeviceContext& context, Buffer* indexBuffer, uint32 indexOffset )
		{
			ID3D11Buffer* buffer = nullptr;
			DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

			if ( auto d3d11buffer = static_cast<D3D11Buffer*>( indexBuffer ) )
			{
				buffer = d3d11buffer->Resource( );
				format = ( d3d11buffer->Stride( ) == 4 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
			}

			context.IASetIndexBuffer( buffer, format, indexOffset );
		}

		static void BindPipelineState( ID3D11DeviceContext& context, PipelineState* pipelineState )
		{
			if ( auto d3d11PipelineState = static_cast<D3D11PipelineState*>( pipelineState ) )
			{
				context.IASetInputLayout( d3d11PipelineState->InputLayout( ) );
				context.IASetPrimitiveTopology( d3d11PipelineState->PrimitiveTopology( ) );
				context.VSSetShader( d3d11PipelineState->VertexShader( ), nullptr, 0 );
				context.PSSetShader( d3d11PipelineState->PixelShader( ), nullptr, 0 );

				float blendFactor[4] = {} /*Temp*/;
				context.OMSetBlendState( d3d11PipelineState->BlendState( ), blendFactor, d3d11PipelineState->SampleMask( ) );
				context.RSSetState( d3d11PipelineState->RasterizerState( ) );
				context.OMSetDepthStencilState( d3d11PipelineState->DepthStencilState( ), 0/*Temp*/ );
			}
		}

		static void BindShaderResources( ID3D11DeviceContext& context, const ShaderBindings& shaderBindings )
		{
			auto setConstantBuffer = [&context]( SHADER_TYPE shader, uint32 slot, const RefHandle<GraphicsApiResource>& cb )
			{
				ID3D11Buffer* buffer = nullptr;
				if ( auto d3d11Buffer = static_cast<D3D11Buffer*>( cb.Get( ) ) )
				{
					buffer = d3d11Buffer->Resource( );
				}

				switch ( shader )
				{
				case SHADER_TYPE::VS:
					context.VSSetConstantBuffers( slot, 1, &buffer );
					break;
				case SHADER_TYPE::HS:
					break;
				case SHADER_TYPE::DS:
					break;
				case SHADER_TYPE::GS:
					break;
				case SHADER_TYPE::PS:
					context.PSSetConstantBuffers( slot, 1, &buffer );
					break;
				default:
					break;
				}
			};

			auto setSRV = [&context]( SHADER_TYPE shader, uint32 slot, const RefHandle<GraphicsApiResource>& srv )
			{
				ID3D11ShaderResourceView* rawSrv = nullptr;
				if ( auto d3d11Srv = reinterpret_cast<D3D11ShaderResourceView*>( srv.Get( ) ) )
				{
					rawSrv = d3d11Srv->Resource( );
				}

				switch ( shader )
				{
				case SHADER_TYPE::VS:
					context.VSSetShaderResources( slot, 1, &rawSrv );
					break;
				case SHADER_TYPE::HS:
					break;
				case SHADER_TYPE::DS:
					break;
				case SHADER_TYPE::GS:
					break;
				case SHADER_TYPE::PS:
					context.PSSetShaderResources( slot, 1, &rawSrv );
					break;
				default:
					break;
				}
			};

			auto setUAV = [&context]( SHADER_TYPE shader, uint32 slot, const RefHandle<GraphicsApiResource>& uav )
			{
				ID3D11UnorderedAccessView* rawUAV = nullptr;
				if ( auto d3d11Uav = reinterpret_cast<D3D11UnorderedAccessView*>( uav.Get( ) ) )
				{
					rawUAV = d3d11Uav->Resource( );
				}

				switch ( shader )
				{
				case SHADER_TYPE::CS:
					context.CSSetUnorderedAccessViews( slot, 1, &rawUAV, nullptr );
					break;
				default:
					break;
				}
			};

			auto setSampler = [&context]( SHADER_TYPE shader, uint32 slot, const RefHandle<GraphicsApiResource>& sampler )
			{
				ID3D11SamplerState* samplerState = nullptr;
				if ( auto d3d11Sampler = static_cast<D3D11SamplerState*>( sampler.Get( ) ) )
				{
					samplerState = d3d11Sampler->Resource( );
				}

				switch ( shader )
				{
				case SHADER_TYPE::VS:
					context.VSSetSamplers( slot, 1, &samplerState );
					break;
				case SHADER_TYPE::HS:
					break;
				case SHADER_TYPE::DS:
					break;
				case SHADER_TYPE::GS:
					break;
				case SHADER_TYPE::PS:
					context.PSSetSamplers( slot, 1, &samplerState );
					break;
				default:
					break;
				}
			};

			for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
			{
				SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<SHADER_TYPE>( shaderType ) );

				const ShaderParameterInfo& parameterInfo = binding.ParameterInfo( );
				for ( size_t i = 0; i < parameterInfo.m_constantBuffers.size( ); ++i )
				{
					const ShaderParameter& param = parameterInfo.m_constantBuffers[i];
					setConstantBuffer( param.m_shader, param.m_bindPoint, binding.GetConstantBufferStart( )[i] );
				}

				for ( size_t i = 0; i < parameterInfo.m_srvs.size( ); ++i )
				{
					const ShaderParameter& param = parameterInfo.m_srvs[i];
					setSRV( param.m_shader, param.m_bindPoint, binding.GetSRVStart( )[i] );
				}

				for ( size_t i = 0; i < parameterInfo.m_uavs.size( ); ++i )
				{
					const ShaderParameter& param = parameterInfo.m_uavs[i];
					setUAV( param.m_shader, param.m_bindPoint, binding.GetUAVStart( )[i] );
				}

				for ( size_t i = 0; i < parameterInfo.m_samplers.size( ); ++i )
				{
					const ShaderParameter& param = parameterInfo.m_samplers[i];
					setSampler( param.m_shader, param.m_bindPoint, binding.GetSamplerStart( )[i] );
				}
			}
		}

		static void Draw( ID3D11DeviceContext& context, uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation )
		{
			context.DrawIndexed( indexCount, startIndexLocation, baseVertexLocation );
		}

		static void DrawInstancing( ID3D11DeviceContext& context, uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
		{
			context.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
		}

		static void SetViewports( ID3D11DeviceContext& context, uint32 count, const CubeArea<float>* area )
		{
			D3D11_VIEWPORT viewports[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

			for ( uint32 i = 0; i < count; ++i )
			{
				viewports[i] = {
					area[i].m_left,
					area[i].m_top,
					area[i].m_right - area[i].m_left,
					area[i].m_bottom - area[i].m_top,
					area[i].m_back,
					area[i].m_front
				};
			}

			context.RSSetViewports( count, viewports );
		}

		static void SetScissorRects( ID3D11DeviceContext& context, uint32 count, const RectangleArea<int32>* area )
		{
			D3D11_RECT rects[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

			for ( uint32 i = 0; i < count; ++i )
			{
				rects[i] = {
					area[i].m_left,
					area[i].m_top,
					area[i].m_right,
					area[i].m_bottom
				};
			}

			context.RSSetScissorRects( count, rects );
		}

		static void BindRenderTargets( ID3D11DeviceContext& context, aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil )
		{
			ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

			for ( uint32 i = 0; i < renderTargetCount; ++i )
			{
				auto rtTex = static_cast<D3D11BaseTexture*>( pRenderTargets[i] );
				if ( rtTex == nullptr )
				{
					continue;
				}

				if ( auto d3d11RTV = static_cast<D3D11RenderTargetView*>( rtTex->RTV( ) ) )
				{
					rtvs[i] = d3d11RTV->Resource( );
				}
			}

			ID3D11DepthStencilView* dsv = nullptr;

			if ( auto dsTex = static_cast<D3D11BaseTexture*>( depthStencil ) )
			{
				if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( dsTex->DSV( ) ) )
				{
					dsv = d3d11DSV->Resource( );
				}
			}

			context.OMSetRenderTargets( renderTargetCount, rtvs, dsv );
		}
	};

	void D3D11ImmediateCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		D3D11CommandList::BindVertexBuffer( D3D11Context( ), vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D11ImmediateCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		D3D11CommandList::BindIndexBuffer( D3D11Context( ), indexBuffer, indexOffset );
	}

	void D3D11ImmediateCommandList::BindPipelineState( PipelineState* pipelineState )
	{
		D3D11CommandList::BindPipelineState( D3D11Context( ), pipelineState );
	}

	void D3D11ImmediateCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
		D3D11CommandList::BindShaderResources( D3D11Context( ), shaderBindings );
	}

	void D3D11ImmediateCommandList::Draw( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		D3D11CommandList::Draw( D3D11Context( ), indexCount, startIndexLocation, baseVertexLocation );
	}

	void D3D11ImmediateCommandList::DrawInstancing( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		D3D11CommandList::DrawInstancing( D3D11Context( ), indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void D3D11ImmediateCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		D3D11CommandList::SetViewports( D3D11Context( ), count, areas );
	}

	void D3D11ImmediateCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		D3D11CommandList::SetScissorRects( D3D11Context( ), count, areas );
	}

	void D3D11ImmediateCommandList::BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil )
	{
		D3D11CommandList::BindRenderTargets( D3D11Context( ), pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11ImmediateCommandList::Execute( IDeferredCommandList& commandList )
	{
		D3D11DeferredCommandList& d3d11DeferredCommandList = static_cast<D3D11DeferredCommandList&>( commandList );
		d3d11DeferredCommandList.RequestExecute( );
	}

	void D3D11DeferredCommandList::BindVertexBuffer( Buffer * const * vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32 * pOffsets )
	{
		D3D11CommandList::BindVertexBuffer( *m_pContext, vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D11DeferredCommandList::BindIndexBuffer( Buffer * indexBuffer, uint32 indexOffset )
	{
		D3D11CommandList::BindIndexBuffer( *m_pContext, indexBuffer, indexOffset );
	}

	void D3D11DeferredCommandList::BindPipelineState( PipelineState * pipelineState )
	{
		D3D11CommandList::BindPipelineState( *m_pContext, pipelineState );
	}

	void D3D11DeferredCommandList::BindShaderResources( const ShaderBindings & shaderBindings )
	{
		D3D11CommandList::BindShaderResources( *m_pContext, shaderBindings );
	}

	void D3D11DeferredCommandList::Draw( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		D3D11CommandList::Draw( *m_pContext, indexCount, startIndexLocation, baseVertexLocation );
	}

	void D3D11DeferredCommandList::DrawInstancing( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		D3D11CommandList::DrawInstancing( *m_pContext, indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void D3D11DeferredCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		D3D11CommandList::SetViewports( *m_pContext, count, areas );
	}

	void D3D11DeferredCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		D3D11CommandList::SetScissorRects( *m_pContext, count, areas );
	}

	void D3D11DeferredCommandList::BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil )
	{
		D3D11CommandList::BindRenderTargets( *m_pContext, pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11DeferredCommandList::Finish( )
	{
		m_pContext->FinishCommandList( false, &m_pCommandList );
	}

	void D3D11DeferredCommandList::RequestExecute( )
	{
		if ( m_pCommandList )
		{
			D3D11Context( ).ExecuteCommandList( m_pCommandList, false );
			m_pCommandList->Release( );
			m_pCommandList = nullptr;
		}
	}

	D3D11DeferredCommandList::D3D11DeferredCommandList( )
	{
		[[maybe_unused]] HRESULT hr = D3D11Device( ).CreateDeferredContext( 0, &m_pContext );
		assert( SUCCEEDED( hr ) );
	}

	D3D11DeferredCommandList::~D3D11DeferredCommandList( )
	{
		if ( m_pContext )
		{
			m_pContext->Release();
			m_pContext = nullptr;
		}
	}
}
