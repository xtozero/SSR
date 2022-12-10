#include "D3D12RasterizerState.h"

#include "D3D12FlagConvertor.h"

using ::agl::ConvertToCullMode;
using ::agl::ConvertToFillMode;
using ::agl::RASTERIZER_STATE_TRAIT;

namespace
{
	D3D12_RASTERIZER_DESC ConvertTraitToDesc( const RASTERIZER_STATE_TRAIT& trait )
	{
		return D3D12_RASTERIZER_DESC{
			.FillMode = ConvertToFillMode( trait.m_fillMode ),
			.CullMode = ConvertToCullMode( trait.m_cullMode ),
			.FrontCounterClockwise = trait.m_frontCounterClockwise,
			.DepthBias = trait.m_depthBias,
			.DepthBiasClamp = trait.m_depthBiasClamp,
			.SlopeScaledDepthBias = trait.m_slopeScaleDepthBias,
			.DepthClipEnable = trait.m_depthClipEnable,
			.MultisampleEnable = trait.m_multisampleEnalbe,
			.AntialiasedLineEnable = trait.m_antialiasedLineEnable,
			.ForcedSampleCount = 0,
			.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
		};
	}
}

namespace agl
{
	D3D12RasterizerState::D3D12RasterizerState( const RASTERIZER_STATE_TRAIT& trait ) 
		: m_desc( ConvertTraitToDesc( trait ) )
	{
	}

	void D3D12RasterizerState::InitResource()
	{
	}

	void D3D12RasterizerState::FreeResource()
	{
	}
}
