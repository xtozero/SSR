#include "D3D12DepthStencilState.h"

#include "D3D12FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToDepthWriteMask;
using ::agl::ConvertToStencilOp;
using ::agl::DepthStencilStateTrait;

namespace
{
	D3D12_DEPTH_STENCIL_DESC ConvertTraitToDesc( const DepthStencilStateTrait& trait )
	{
		return D3D12_DEPTH_STENCIL_DESC{
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
	D3D12DepthStencilState::D3D12DepthStencilState( const DepthStencilStateTrait& trait ) 
		: m_desc( ConvertTraitToDesc( trait ) )
	{
	}

	void D3D12DepthStencilState::InitResource()
	{
	}

	void D3D12DepthStencilState::FreeResource()
	{
	}
}
