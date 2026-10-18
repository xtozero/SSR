#include "D3D12BlendState.h"

#include "D3D12FlagConvertor.h"

using ::agl::BlendStateDesc;
using ::agl::D3D12FlagConvertor;
using ::agl::ConvertToColorWriteEnable;
using ::agl::MaxRendertagets;

namespace
{
	D3D12_BLEND_DESC ConvertToD3DDesc( const BlendStateDesc& desc )
	{
		D3D12_BLEND_DESC d3dDesc = {
			.AlphaToCoverageEnable = desc.m_alphaToConverageEnable,
			.IndependentBlendEnable = desc.m_independentBlendEnable
		};

		for ( uint32 i = 0; i < MaxRendertagets; ++i )
		{
			d3dDesc.RenderTarget[i] = {
				.BlendEnable = desc.m_renderTarget[i].m_blendEnable,
				.LogicOpEnable = false,
				.SrcBlend = D3D12FlagConvertor::ConvertToBlend( desc.m_renderTarget[i].m_srcBlend ),
				.DestBlend = D3D12FlagConvertor::ConvertToBlend( desc.m_renderTarget[i].m_destBlend ),
				.BlendOp = D3D12FlagConvertor::ConvertToBlendOp( desc.m_renderTarget[i].m_blendOp ),
				.SrcBlendAlpha = D3D12FlagConvertor::ConvertToBlend( desc.m_renderTarget[i].m_srcBlendAlpha ),
				.DestBlendAlpha = D3D12FlagConvertor::ConvertToBlend( desc.m_renderTarget[i].m_destBlendAlpha ),
				.BlendOpAlpha = D3D12FlagConvertor::ConvertToBlendOp( desc.m_renderTarget[i].m_blendOpAlpha ),
				.LogicOp = D3D12_LOGIC_OP_NOOP,
				.RenderTargetWriteMask = ConvertToColorWriteEnable( desc.m_renderTarget[i].m_renderTargetWriteMask )
			};
		}

		return d3dDesc;
	}
}

namespace agl
{
	const float* D3D12BlendState::GetBlendFactor() const
	{
		return m_blendFactor;
	}

	uint32 D3D12BlendState::SamplerMask() const
	{
		return m_sampleMask;
	}

	void D3D12BlendState::SetBlendFactor( const float( &blendFactor )[4] )
	{
		for ( uint32 i = 0; i < 4; ++i )
		{
			m_blendFactor[i] = blendFactor[i];
		}
	}

	void D3D12BlendState::SetSampleMask( uint32 sampleMask )
	{
		m_sampleMask = sampleMask;
	}

	D3D12BlendState::D3D12BlendState( const BlendStateDesc& desc )
		: m_sampleMask( desc.m_sampleMask )
		, m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
	}

	void D3D12BlendState::InitResource()
	{
	}

	void D3D12BlendState::FreeResource()
	{
	}
}
