#include "D3D11DepthStencilState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToStencilOp;
using ::agl::ConvertToDepthWriteMask;
using ::agl::DepthStencilStateDesc;

namespace
{
	D3D11_DEPTH_STENCIL_DESC ConvertToD3DDesc( const DepthStencilStateDesc& desc )
	{
		return D3D11_DEPTH_STENCIL_DESC{
			.DepthEnable = desc.m_depthEnable,
			.DepthWriteMask = ConvertToDepthWriteMask( desc.m_depthWriteMode ),
			.DepthFunc = ConvertToComparisionFunc( desc.m_depthFunc ),
			.StencilEnable = desc.m_stencilEnable,
			.StencilReadMask = desc.m_stencilReadMask,
			.StencilWriteMask = desc.m_stencilWriteMask,
			.FrontFace = {
				.StencilFailOp = ConvertToStencilOp( desc.m_frontFace.m_failOp ),
				.StencilDepthFailOp = ConvertToStencilOp( desc.m_frontFace.m_depthFailOp ),
				.StencilPassOp = ConvertToStencilOp( desc.m_frontFace.m_passOp ),
				.StencilFunc = ConvertToComparisionFunc( desc.m_frontFace.m_func )
			},
			.BackFace = {
				.StencilFailOp = ConvertToStencilOp( desc.m_backFace.m_failOp ),
				.StencilDepthFailOp = ConvertToStencilOp( desc.m_backFace.m_depthFailOp ),
				.StencilPassOp = ConvertToStencilOp( desc.m_backFace.m_passOp ),
				.StencilFunc = ConvertToComparisionFunc( desc.m_backFace.m_func )
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

	D3D11DepthStencilState::D3D11DepthStencilState( const DepthStencilStateDesc& desc ) : m_d3dDesc( ConvertToD3DDesc( desc ) ) {}

	void D3D11DepthStencilState::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateDepthStencilState( &m_d3dDesc, &m_depthStencilState ) );
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
