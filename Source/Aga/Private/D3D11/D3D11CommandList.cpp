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
	void D3D11ImmediateCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
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

		D3D11Context( ).IASetVertexBuffers( startSlot, numBuffers, pBuffers, strides, offsets );
	}

	void D3D11ImmediateCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		ID3D11Buffer* buffer = nullptr;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		if ( auto d3d11buffer = static_cast<D3D11Buffer*>( indexBuffer ) )
		{
			buffer = d3d11buffer->Resource( );
			format = ( d3d11buffer->Stride( ) == 4 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		}

		D3D11Context( ).IASetIndexBuffer( buffer, format, indexOffset );
	}

	void D3D11ImmediateCommandList::BindPipelineState( PipelineState* pipelineState )
	{
		if ( auto d3d11PipelineState = static_cast<D3D11PipelineState*>( pipelineState ) )
		{
			auto& context = D3D11Context( );

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

	void D3D11ImmediateCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
		auto& context = D3D11Context( );

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
				rawSrv = d3d11Srv->Resource();
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

	void D3D11ImmediateCommandList::Draw( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		auto& context = D3D11Context( );
		context.DrawIndexed( indexCount, startIndexLocation, baseVertexLocation );
	}

	void D3D11ImmediateCommandList::DrawInstancing( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		auto& context = D3D11Context( );
		context.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
	}
}
