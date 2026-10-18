#include "D3D12DepthStencilState.h"

#include "D3D12FlagConvertor.h"

using ::agl::D3D12FlagConvertor;
using ::agl::DepthStencilStateDesc;

namespace
{
	D3D12_DEPTH_STENCIL_DESC ConvertToD3DDesc( const DepthStencilStateDesc& desc )
	{
		return D3D12_DEPTH_STENCIL_DESC{
			.DepthEnable = desc.m_depthEnable,
			.DepthWriteMask = D3D12FlagConvertor::ConvertToDepthWriteMask( desc.m_depthWriteMode ),
			.DepthFunc = D3D12FlagConvertor::ConvertToComparisionFunc( desc.m_depthFunc ),
			.StencilEnable = desc.m_stencilEnable,
			.StencilReadMask = desc.m_stencilReadMask,
			.StencilWriteMask = desc.m_stencilWriteMask,
			.FrontFace = {
				.StencilFailOp = D3D12FlagConvertor::ConvertToStencilOp( desc.m_frontFace.m_failOp ),
				.StencilDepthFailOp = D3D12FlagConvertor::ConvertToStencilOp( desc.m_frontFace.m_depthFailOp ),
				.StencilPassOp = D3D12FlagConvertor::ConvertToStencilOp( desc.m_frontFace.m_passOp ),
				.StencilFunc = D3D12FlagConvertor::ConvertToComparisionFunc( desc.m_frontFace.m_func )
			},
			.BackFace = {
				.StencilFailOp = D3D12FlagConvertor::ConvertToStencilOp( desc.m_backFace.m_failOp ),
				.StencilDepthFailOp = D3D12FlagConvertor::ConvertToStencilOp( desc.m_backFace.m_depthFailOp ),
				.StencilPassOp = D3D12FlagConvertor::ConvertToStencilOp( desc.m_backFace.m_passOp ),
				.StencilFunc = D3D12FlagConvertor::ConvertToComparisionFunc( desc.m_backFace.m_func )
			}
		};
	}
}

namespace agl
{
	D3D12DepthStencilState::D3D12DepthStencilState( const DepthStencilStateDesc& desc )
		: m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
	}

	void D3D12DepthStencilState::InitResource()
	{
	}

	void D3D12DepthStencilState::FreeResource()
	{
	}
}
