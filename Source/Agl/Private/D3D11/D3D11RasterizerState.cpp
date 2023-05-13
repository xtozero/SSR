#include "stdafx.h"
#include "D3D11RasterizerState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::ConvertToCullMode;
using ::agl::ConvertToFillMode;
using ::agl::RasterizerStateTrait;

namespace
{
	D3D11_RASTERIZER_DESC ConvertTraitToDesc( const RasterizerStateTrait& trait )
	{
		return D3D11_RASTERIZER_DESC{
			.FillMode = ConvertToFillMode( trait.m_fillMode ),
			.CullMode = ConvertToCullMode( trait.m_cullMode ),
			.FrontCounterClockwise = trait.m_frontCounterClockwise,
			.DepthBias = trait.m_depthBias,
			.DepthBiasClamp = trait.m_depthBiasClamp,
			.SlopeScaledDepthBias = trait.m_slopeScaleDepthBias,
			.DepthClipEnable = trait.m_depthClipEnable,
			.ScissorEnable = trait.m_scissorEnable,
			.MultisampleEnable = trait.m_multisampleEnable,
			.AntialiasedLineEnable = trait.m_antialiasedLineEnable
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

	D3D11RasterizerState::D3D11RasterizerState( const RasterizerStateTrait& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

	void D3D11RasterizerState::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateRasterizerState( &m_desc, &m_rasterizerState ) );
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
