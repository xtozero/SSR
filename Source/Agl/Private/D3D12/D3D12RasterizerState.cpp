#include "D3D12RasterizerState.h"

#include "D3D12FlagConvertor.h"

using ::agl::ConvertToCullMode;
using ::agl::ConvertToFillMode;
using ::agl::RasterizerStateDesc;

namespace
{
	D3D12_RASTERIZER_DESC ConvertToD3DDesc( const RasterizerStateDesc& desc )
	{
		return D3D12_RASTERIZER_DESC{
			.FillMode = ConvertToFillMode( desc.m_fillMode ),
			.CullMode = ConvertToCullMode( desc.m_cullMode ),
			.FrontCounterClockwise = desc.m_frontCounterClockwise,
			.DepthBias = desc.m_depthBias,
			.DepthBiasClamp = desc.m_depthBiasClamp,
			.SlopeScaledDepthBias = desc.m_slopeScaleDepthBias,
			.DepthClipEnable = desc.m_depthClipEnable,
			.MultisampleEnable = desc.m_multisampleEnable,
			.AntialiasedLineEnable = desc.m_antialiasedLineEnable,
			.ForcedSampleCount = 0,
			.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
		};
	}
}

namespace agl
{
	D3D12RasterizerState::D3D12RasterizerState( const RasterizerStateDesc& desc )
		: m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
	}

	void D3D12RasterizerState::InitResource()
	{
	}

	void D3D12RasterizerState::FreeResource()
	{
	}
}
