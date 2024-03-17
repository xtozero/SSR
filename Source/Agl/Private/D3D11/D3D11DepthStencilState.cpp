#include "D3D11DepthStencilState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToStencilOp;
using ::agl::ConvertToDepthWriteMask;
using ::agl::DepthStencilStateTrait;

namespace
{
	D3D11_DEPTH_STENCIL_DESC ConvertTraitToDesc( const DepthStencilStateTrait& trait )
	{
		return D3D11_DEPTH_STENCIL_DESC{
			.DepthEnable = trait.m_depthEnable,
			.DepthWriteMask = ConvertToDepthWriteMask( trait.m_depthWriteMode ),
			.DepthFunc = ConvertToComparisionFunc( trait.m_depthFunc ),
			.StencilEnable = trait.m_stencilEnable,
			.StencilReadMask = trait.m_stencilReadMask,
			.StencilWriteMask = trait.m_stencilWriteMask,
			.FrontFace = {
				.StencilFailOp = ConvertToStencilOp( trait.m_frontFace.m_failOp ),
				.StencilDepthFailOp = ConvertToStencilOp( trait.m_frontFace.m_depthFailOp ),
				.StencilPassOp = ConvertToStencilOp( trait.m_frontFace.m_passOp ),
				.StencilFunc = ConvertToComparisionFunc( trait.m_frontFace.m_func )
			},
			.BackFace = {
				.StencilFailOp = ConvertToStencilOp( trait.m_backFace.m_failOp ),
				.StencilDepthFailOp = ConvertToStencilOp( trait.m_backFace.m_depthFailOp ),
				.StencilPassOp = ConvertToStencilOp( trait.m_backFace.m_passOp ),
				.StencilFunc = ConvertToComparisionFunc( trait.m_backFace.m_func )
			}
		};
	}
}

namespace agl
{
	ID3D11DepthStencilState* D3D11DepthStencilState::Resource()
	{
		return m_depthStencilState;
	}

	const ID3D11DepthStencilState* D3D11DepthStencilState::Resource() const
	{
		return m_depthStencilState;
	}

	uint32 D3D11DepthStencilState::GetStencilRef() const
	{
		return m_stencilRef;
	}

	void D3D11DepthStencilState::SetStencilRef( uint32 stencilRef )
	{
		m_stencilRef = stencilRef;
	}

	D3D11DepthStencilState::D3D11DepthStencilState( const DepthStencilStateTrait& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

	void D3D11DepthStencilState::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateDepthStencilState( &m_desc, &m_depthStencilState ) );
		assert( result );
	}

	void D3D11DepthStencilState::FreeResource()
	{
		if ( m_depthStencilState )
		{
			m_depthStencilState->Release();
			m_depthStencilState = nullptr;
		}
	}
}
