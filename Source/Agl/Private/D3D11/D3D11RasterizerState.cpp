#include "D3D11RasterizerState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::ConvertToCullMode;
using ::agl::ConvertToFillMode;
using ::agl::RasterizerStateDesc;

namespace
{
	D3D11_RASTERIZER_DESC ConvertToD3DDesc( const RasterizerStateDesc& desc )
	{
		return D3D11_RASTERIZER_DESC{
			.FillMode = ConvertToFillMode( desc.m_fillMode ),
			.CullMode = ConvertToCullMode( desc.m_cullMode ),
			.FrontCounterClockwise = desc.m_frontCounterClockwise,
			.DepthBias = desc.m_depthBias,
			.DepthBiasClamp = desc.m_depthBiasClamp,
			.SlopeScaledDepthBias = desc.m_slopeScaleDepthBias,
			.DepthClipEnable = desc.m_depthClipEnable,
			.ScissorEnable = desc.m_scissorEnable,
			.MultisampleEnable = desc.m_multisampleEnable,
			.AntialiasedLineEnable = desc.m_antialiasedLineEnable
		};
	}
}

namespace agl
{
	ID3D11RasterizerState* D3D11RasterizerState::Resource()
	{
		return m_rasterizerState;
	}

	const ID3D11RasterizerState* D3D11RasterizerState::Resource() const
	{
		return m_rasterizerState;
	}

	D3D11RasterizerState::D3D11RasterizerState( const RasterizerStateDesc& desc ) : m_d3dDesc( ConvertToD3DDesc( desc ) ) {}

	void D3D11RasterizerState::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateRasterizerState( &m_d3dDesc, &m_rasterizerState ) );
		assert( result );
	}

	void D3D11RasterizerState::FreeResource()
	{
		if ( m_rasterizerState )
		{
			m_rasterizerState->Release();
			m_rasterizerState = nullptr;
		}
	}
}
