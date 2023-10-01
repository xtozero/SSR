#include "D3D12BlendState.h"

#include "D3D12FlagConvertor.h"

using ::agl::BlendStateTrait;
using ::agl::ConvertToBlend;
using ::agl::ConvertToBlendOp;
using ::agl::ConvertToColorWriteEnable;
using ::agl::MAX_RENDER_TARGET;

namespace
{
	D3D12_BLEND_DESC ConvertTraitToDesc( const BlendStateTrait& trait )
	{
		D3D12_BLEND_DESC desc = {
			.AlphaToCoverageEnable = trait.m_alphaToConverageEnable,
			.IndependentBlendEnable = trait.m_independentBlendEnable
		};

		for ( uint32 i = 0; i < MAX_RENDER_TARGET; ++i )
		{
			desc.RenderTarget[i] = {
				.BlendEnable = trait.m_renderTarget[i].m_blendEnable,
				.LogicOpEnable = false,
				.SrcBlend = ConvertToBlend( trait.m_renderTarget[i].m_srcBlend ),
				.DestBlend = ConvertToBlend( trait.m_renderTarget[i].m_destBlend ),
				.BlendOp = ConvertToBlendOp( trait.m_renderTarget[i].m_blendOp ),
				.SrcBlendAlpha = ConvertToBlend( trait.m_renderTarget[i].m_srcBlendAlpha ),
				.DestBlendAlpha = ConvertToBlend( trait.m_renderTarget[i].m_destBlendAlpha ),
				.BlendOpAlpha = ConvertToBlendOp( trait.m_renderTarget[i].m_blendOpAlpha ),
				.LogicOp = D3D12_LOGIC_OP_NOOP,
				.RenderTargetWriteMask = ConvertToColorWriteEnable( trait.m_renderTarget[i].m_renderTargetWriteMask )
			};
		}

		return desc;
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

	D3D12BlendState::D3D12BlendState( const BlendStateTrait& trait ) 
		: m_sampleMask( trait.m_sampleMask )
		, m_desc( ConvertTraitToDesc( trait ) )
	{
	}

	void D3D12BlendState::InitResource()
	{
	}

	void D3D12BlendState::FreeResource()
	{
	}
}
