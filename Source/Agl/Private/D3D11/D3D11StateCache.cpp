#include "stdafx.h"
#include "D3D11StateCache.h"

#include "D3D11BaseTexture.h"
#include "D3D11Buffer.h"
#include "D3D11PipelineState.h"
#include "D3D11ResourceViews.h"
#include "D3D11SamplerState.h"
#include "D3D11Shaders.h"
#include "ShaderBindings.h"

#include <algorithm>

namespace agl
{
	void D3D11PipelineCache::BindVertexBuffer( ID3D11DeviceContext& context, Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		ID3D11Buffer* pBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
		uint32 strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
		uint32 offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

		if ( vertexBuffers )
		{
			assert( pOffsets != nullptr );
			for ( uint32 i = 0; i < numBuffers; ++i )
			{
				auto d3d11buffer = static_cast<D3D11Buffer*>( vertexBuffers[i] );
				if ( d3d11buffer )
				{
					pBuffers[i] = d3d11buffer->Resource();
					strides[i] = d3d11buffer->Stride();
					offsets[i] = pOffsets[i];
				}
			}
		}

		static_assert( sizeof( pBuffers ) == sizeof( m_vertexBuffers ) );
		static_assert( sizeof( strides ) == sizeof( m_vertexStrides ) );
		static_assert( sizeof( offsets ) == sizeof( m_vertexOffsets ) );
		if ( std::equal( std::begin( pBuffers ), std::end( pBuffers ), std::begin( m_vertexBuffers ) ) &&
			std::equal( std::begin( strides ), std::end( strides ), std::begin( m_vertexStrides ) ) &&
			std::equal( std::begin( offsets ), std::end( offsets ), std::begin( m_vertexOffsets ) ) )
		{
			return;
		}

		if ( numBuffers == 0 )
		{
			numBuffers = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
		}

		std::copy( std::begin( pBuffers ), std::end( pBuffers ), std::begin( m_vertexBuffers ) );
		std::copy( std::begin( strides ), std::end( strides ), std::begin( m_vertexStrides ) );
		std::copy( std::begin( offsets ), std::end( offsets ), std::begin( m_vertexOffsets ) );
		context.IASetVertexBuffers( startSlot, numBuffers, pBuffers, strides, offsets );
	}

	void D3D11PipelineCache::BindIndexBuffer( ID3D11DeviceContext& context, Buffer* indexBuffer, uint32 indexOffset )
	{
		ID3D11Buffer* buffer = nullptr;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		if ( auto d3d11buffer = static_cast<D3D11Buffer*>( indexBuffer ) )
		{
			buffer = d3d11buffer->Resource();
			format = ( d3d11buffer->Stride() == 4 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		}

		if ( m_indexBuffer == buffer && m_indexBufferFormat == format )
		{
			return;
		}

		m_indexBuffer = buffer;
		m_indexBufferFormat = format;
		context.IASetIndexBuffer( buffer, format, indexOffset );
	}

	void D3D11PipelineCache::BindPipelineState( ID3D11DeviceContext& context, PipelineState* pipelineState )
	{
		ID3D11InputLayout* inputLayout = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11GeometryShader* geometryShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11BlendState* blendState = nullptr;
		uint32 blendSampleMask = 0;
		ID3D11RasterizerState* rasterizerState = nullptr;
		ID3D11DepthStencilState* depthStencilState = nullptr;

		if ( auto d3d11PipelineState = static_cast<D3D11PipelineState*>( pipelineState ) )
		{
			inputLayout = d3d11PipelineState->InputLayout();
			topology = d3d11PipelineState->PrimitiveTopology();
			vertexShader = d3d11PipelineState->VertexShader();
			geometryShader = d3d11PipelineState->GeometryShader();
			pixelShader = d3d11PipelineState->PixelShader();
			blendState = d3d11PipelineState->BlendState();
			blendSampleMask = d3d11PipelineState->SampleMask();
			rasterizerState = d3d11PipelineState->RasterizerState();
			depthStencilState = d3d11PipelineState->DepthStencilState();
		}

		if ( inputLayout != m_inputLayout )
		{
			m_inputLayout = inputLayout;
			context.IASetInputLayout( inputLayout );
		}

		if ( topology != m_primitiveTopology )
		{
			m_primitiveTopology = topology;
			context.IASetPrimitiveTopology( topology );
		}

		if ( vertexShader != m_vertexShader )
		{
			m_vertexShader = vertexShader;
			context.VSSetShader( vertexShader, nullptr, 0 );
		}

		if ( geometryShader != m_geometryShader )
		{
			m_geometryShader = geometryShader;
			context.GSSetShader( geometryShader, nullptr, 0 );
		}

		if ( pixelShader != m_pixelShader )
		{
			m_pixelShader = pixelShader;
			context.PSSetShader( pixelShader, nullptr, 0 );
		}

		if ( blendState != m_blendState ||
			blendSampleMask != m_blendSampleMask )
		{
			m_blendState = blendState;
			m_blendSampleMask = blendSampleMask;
			float blendFactor[4] = {} /*Temp*/;
			context.OMSetBlendState( blendState, blendFactor, blendSampleMask );
		}

		if ( rasterizerState != m_rasterizerState )
		{
			m_rasterizerState = rasterizerState;
			context.RSSetState( rasterizerState );
		}

		if ( depthStencilState != m_depthStencilState )
		{
			m_depthStencilState = depthStencilState;
			context.OMSetDepthStencilState( depthStencilState, 0/*Temp*/ );
		}
	}

	void D3D11PipelineCache::BindShader( ID3D11DeviceContext& context, VertexShader* vs )
	{
		ID3D11VertexShader* vertexShader = nullptr;

		if ( auto d3d11VS = static_cast<D3D11VertexShader*>( vs ) )
		{
			vertexShader = d3d11VS->Resource();
		}

		if ( vertexShader != m_vertexShader )
		{
			m_vertexShader = vertexShader;
			context.VSSetShader( vertexShader, nullptr, 0 );
		}
	}

	void D3D11PipelineCache::BindShader( ID3D11DeviceContext& context, GeometryShader* gs )
	{
		ID3D11GeometryShader* geometryShader = nullptr;

		if ( auto d3d11GS = static_cast<D3D11GeometryShader*>( gs ) )
		{
			geometryShader = d3d11GS->Resource();
		}

		if ( geometryShader != m_geometryShader )
		{
			m_geometryShader = geometryShader;
			context.GSSetShader( geometryShader, nullptr, 0 );
		}
	}

	void D3D11PipelineCache::BindShader( ID3D11DeviceContext& context, PixelShader* ps )
	{
		ID3D11PixelShader* pixelShader = nullptr;

		if ( auto d3d11PS = static_cast<D3D11PixelShader*>( ps ) )
		{
			pixelShader = d3d11PS->Resource();
		}

		if ( pixelShader != m_pixelShader )
		{
			m_pixelShader = pixelShader;
			context.PSSetShader( pixelShader, nullptr, 0 );
		}
	}

	void D3D11PipelineCache::BindShader( ID3D11DeviceContext& context, ComputeShader* cs )
	{
		ID3D11ComputeShader* computeShader = nullptr;

		if ( auto d3d11CS = static_cast<D3D11ComputeShader*>( cs ) )
		{
			computeShader = d3d11CS->Resource();
		}

		if ( computeShader != m_computeShader )
		{
			m_computeShader = computeShader;
			context.CSSetShader( computeShader, nullptr, 0 );
		}
	}

	void D3D11PipelineCache::BindShaderResources( ID3D11DeviceContext& context, const ShaderBindings& shaderBindings )
	{
		for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );

			const ShaderParameterInfo& parameterInfo = binding.ParameterInfo();
			for ( size_t i = 0; i < parameterInfo.m_constantBuffers.size(); ++i )
			{
				const ShaderParameter& param = parameterInfo.m_constantBuffers[i];
				BindConstantBuffer( context, param.m_shader, param.m_bindPoint, binding.GetConstantBufferStart()[i] );
			}

			for ( size_t i = 0; i < parameterInfo.m_srvs.size(); ++i )
			{
				const ShaderParameter& param = parameterInfo.m_srvs[i];
				BindSRV( context, param.m_shader, param.m_bindPoint, binding.GetSRVStart()[i] );
			}

			for ( size_t i = 0; i < parameterInfo.m_uavs.size(); ++i )
			{
				const ShaderParameter& param = parameterInfo.m_uavs[i];
				BindUAV( context, param.m_shader, param.m_bindPoint, binding.GetUAVStart()[i] );
			}

			for ( size_t i = 0; i < parameterInfo.m_samplers.size(); ++i )
			{
				const ShaderParameter& param = parameterInfo.m_samplers[i];
				BindSampler( context, param.m_shader, param.m_bindPoint, binding.GetSamplerStart()[i] );
			}
		}
	}

	void D3D11PipelineCache::BindConstantBuffer( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, Buffer* cb )
	{
		ID3D11Buffer* buffer = nullptr;
		if ( auto d3d11Buffer = static_cast<D3D11Buffer*>( cb ) )
		{
			buffer = d3d11Buffer->Resource();
		}

		auto nShader = static_cast<uint32>( shader );
		if ( m_constantBuffers[nShader][slot] == buffer )
		{
			return;
		}

		m_constantBuffers[nShader][slot] = buffer;
		switch ( shader )
		{
		case ShaderType::VS:
			context.VSSetConstantBuffers( slot, 1, &buffer );
			break;
		case ShaderType::HS:
			break;
		case ShaderType::DS:
			break;
		case ShaderType::GS:
			context.GSSetConstantBuffers( slot, 1, &buffer );
			break;
		case ShaderType::PS:
			context.PSSetConstantBuffers( slot, 1, &buffer );
			break;
		case ShaderType::CS:
			context.CSSetConstantBuffers( slot, 1, &buffer );
			break;
		default:
			break;
		}
	}

	void D3D11PipelineCache::BindSRV( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, ShaderResourceView* srv )
	{
		ID3D11ShaderResourceView* rawSrv = nullptr;
		if ( auto d3d11Srv = static_cast<D3D11ShaderResourceView*>( srv ) )
		{
			rawSrv = d3d11Srv->Resource();

			if ( const IResourceViews* sibiling = d3d11Srv->ViewHolder() )
			{
				if ( auto d3d11Uav = static_cast<const D3D11UnorderedAccessView*>(sibiling->UAV()) )
				{
					UnbindExistingUAV( context, d3d11Uav->Resource() );
				}
			}
		}

		auto nShader = static_cast<uint32>( shader );
		if ( m_srvs[nShader][slot] == rawSrv )
		{
			return;
		}

		m_srvs[nShader][slot] = rawSrv;
		switch ( shader )
		{
		case ShaderType::VS:
			context.VSSetShaderResources( slot, 1, &rawSrv );
			break;
		case ShaderType::HS:
			break;
		case ShaderType::DS:
			break;
		case ShaderType::GS:
			context.GSSetShaderResources( slot, 1, &rawSrv );
			break;
		case ShaderType::PS:
			context.PSSetShaderResources( slot, 1, &rawSrv );
			break;
		case ShaderType::CS:
			context.CSSetShaderResources( slot, 1, &rawSrv );
			break;
		default:
			break;
		}
	}

	void D3D11PipelineCache::BindUAV( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, UnorderedAccessView* uav )
	{
		ID3D11UnorderedAccessView* rawUav = nullptr;
		if ( auto d3d11Uav = static_cast<D3D11UnorderedAccessView*>( uav ) )
		{
			rawUav = d3d11Uav->Resource();

			if ( const IResourceViews* sibiling = d3d11Uav->ViewHolder() )
			{
				if ( auto d3d11Srv = static_cast<const D3D11ShaderResourceView*>(sibiling->SRV()) )
				{
					UnbindExistingSRV( context, d3d11Srv->Resource() );
				}
			}
		}

		if ( m_uavs[slot] == rawUav )
		{
			return;
		}

		m_uavs[slot] = rawUav;
		switch ( shader )
		{
		case ShaderType::CS:
			context.CSSetUnorderedAccessViews( slot, 1, &rawUav, nullptr );
			break;
		default:
			break;
		}
	}

	void D3D11PipelineCache::BindSampler( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, SamplerState* sampler )
	{
		ID3D11SamplerState* samplerState = nullptr;
		if ( auto d3d11Sampler = static_cast<D3D11SamplerState*>( sampler ) )
		{
			samplerState = d3d11Sampler->Resource();
		}

		auto nShader = static_cast<uint32>( shader );
		if ( m_samplerStates[nShader][slot] == samplerState )
		{
			return;
		}

		m_samplerStates[nShader][slot] = samplerState;
		switch ( shader )
		{
		case ShaderType::VS:
			context.VSSetSamplers( slot, 1, &samplerState );
			break;
		case ShaderType::HS:
			break;
		case ShaderType::DS:
			break;
		case ShaderType::GS:
			context.GSSetSamplers( slot, 1, &samplerState );
			break;
		case ShaderType::PS:
			context.PSSetSamplers( slot, 1, &samplerState );
			break;
		case ShaderType::CS:
			context.CSSetSamplers( slot, 1, &samplerState );
			break;
		default:
			break;
		}
	}

	void D3D11PipelineCache::SetViewports( ID3D11DeviceContext& context, uint32 count, const CubeArea<float>* area )
	{
		D3D11_VIEWPORT viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};

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

		static_assert( sizeof( viewports ) == sizeof( m_viewports ) );
		if ( std::equal( std::begin( viewports ), std::end( viewports ), std::begin( m_viewports ) ) )
		{
			return;
		}

		std::copy( std::begin( viewports ), std::end( viewports ), std::begin( m_viewports ) );
		context.RSSetViewports( count, viewports );
	}

	void D3D11PipelineCache::SetScissorRects( ID3D11DeviceContext& context, uint32 count, const RectangleArea<int32>* area )
	{
		D3D11_RECT rects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};

		for ( uint32 i = 0; i < count; ++i )
		{
			rects[i] = {
				area[i].m_left,
				area[i].m_top,
				area[i].m_right,
				area[i].m_bottom
			};
		}

		static_assert( sizeof( rects ) == sizeof( m_siccorRects ) );
		if ( std::equal( std::begin( rects ), std::end( rects ), std::begin( m_siccorRects ) ) )
		{
			return;
		}

		std::copy( std::begin( rects ), std::end( rects ), std::begin( m_siccorRects ) );
		context.RSSetScissorRects( count, rects );
	}

	void D3D11PipelineCache::BindRenderTargets( ID3D11DeviceContext& context, RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

		for ( uint32 i = 0; i < renderTargetCount; ++i )
		{
			auto d3d11Rtv = static_cast<D3D11RenderTargetView*>( pRenderTargets[i] );
			if ( d3d11Rtv )
			{
				rtvs[i] = d3d11Rtv->Resource();
			}
			else
			{
				continue;
			}

			if ( const IResourceViews* sibiling = d3d11Rtv->ViewHolder() )
			{
				if ( auto d3d11Srv = static_cast<const D3D11ShaderResourceView*>( sibiling->SRV() ) )
				{
					UnbindExistingSRV( context, d3d11Srv->Resource() );
				}

				if ( auto d3d11Uav = static_cast<const D3D11UnorderedAccessView*>( sibiling->UAV() ) )
				{
					UnbindExistingUAV( context, d3d11Uav->Resource() );
				}
			}
		}

		ID3D11DepthStencilView* dsv = nullptr;

		if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( depthStencil ) )
		{
			dsv = d3d11DSV->Resource();

			if ( const IResourceViews* sibiling = d3d11DSV->ViewHolder() )
			{
				if ( auto d3d11SRV = static_cast<const D3D11ShaderResourceView*>( sibiling->SRV() ) )
				{
					UnbindExistingSRV( context, d3d11SRV->Resource() );
				}

				if ( auto d3d11Uav = static_cast<const D3D11UnorderedAccessView*>( sibiling->UAV() ) )
				{
					UnbindExistingUAV( context, d3d11Uav->Resource() );
				}
			}
		}

		static_assert( sizeof( rtvs ) == sizeof( m_rtvs ) );
		if ( dsv == m_dsv &&
			std::equal( std::begin( rtvs ), std::end( rtvs ), std::begin( m_rtvs ) ) )
		{
			return;
		}

		std::copy( std::begin( rtvs ), std::end( rtvs ), std::begin( m_rtvs ) );
		m_dsv = dsv;
		context.OMSetRenderTargets( renderTargetCount, rtvs, dsv );
	}

	void D3D11PipelineCache::UnbindExistingSRV( ID3D11DeviceContext& context, ID3D11ShaderResourceView* srv )
	{
		for ( uint32 shader = 0; shader < MAX_SHADER_TYPE<uint32>; ++shader )
		{
			for ( uint32 slot = 0; slot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; ++slot )
			{
				if ( m_srvs[shader][slot] == srv )
				{
					BindSRV( context, static_cast<ShaderType>( shader ), slot, RefHandle<ShaderResourceView>() );
				}
			}
		}
	}

	void D3D11PipelineCache::UnbindExistingUAV( ID3D11DeviceContext& context, ID3D11UnorderedAccessView* uav )
	{
		for ( uint32 slot = 0; slot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; ++slot )
		{
			if ( m_uavs[slot] == uav )
			{
				BindUAV( context, ShaderType::CS, slot, RefHandle<UnorderedAccessView>() );
			}
		}
	}
}
