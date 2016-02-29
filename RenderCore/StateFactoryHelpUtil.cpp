#include "stdafx.h"
#include "StateFactoryHelpUtil.h"

#include <D3D11.h>

D3D11_FILL_MODE TranslateFillMode( const String& fillMode )
{
	if ( fillMode == _T( "D3D11_FILL_SOLID" ) )
	{
		return D3D11_FILL_SOLID;
	}
	else if ( fillMode == _T( "D3D11_FILL_WIREFRAME" ) )
	{
		return D3D11_FILL_WIREFRAME;
	}

	return D3D11_FILL_SOLID;
}

D3D11_CULL_MODE TranslateCullMode( const String& cullMode )
{
	if ( cullMode == _T( "D3D11_CULL_NONE" ) )
	{
		return D3D11_CULL_NONE;
	}
	else if ( cullMode == _T( "D3D11_CULL_FRONT" ) )
	{
		return D3D11_CULL_FRONT;
	}
	else if ( cullMode == _T( "D3D11_CULL_BACK" ) )
	{
		return D3D11_CULL_BACK;
	}

	return D3D11_CULL_BACK;
}

D3D11_COMPARISON_FUNC TranslateComparisonFunc( const String& key )
{
	if ( key == _T( "D3D11_COMPARISON_NEVER" ) )
	{
		return D3D11_COMPARISON_NEVER;
	}
	else if ( key == _T( "D3D11_COMPARISON_LESS" ) )
	{
		return D3D11_COMPARISON_LESS;
	}
	else if ( key == _T( "D3D11_COMPARISON_EQUAL" ) )
	{
		return D3D11_COMPARISON_EQUAL;
	}
	else if ( key == _T( "D3D11_COMPARISON_LESS_EQUAL" ) )
	{
		return D3D11_COMPARISON_LESS_EQUAL;
	}
	else if ( key == _T( "D3D11_COMPARISON_GREATER" ) )
	{
		return D3D11_COMPARISON_GREATER;
	}
	else if ( key == _T( "D3D11_COMPARISON_NOT_EQUAL" ) )
	{
		return D3D11_COMPARISON_NOT_EQUAL;
	}
	else if ( key == _T( "D3D11_COMPARISON_GREATER_EQUAL" ) )
	{
		return D3D11_COMPARISON_GREATER_EQUAL;
	}
	else if ( key == _T( "D3D11_COMPARISON_ALWAYS" ) )
	{
		return D3D11_COMPARISON_ALWAYS;
	}

	return D3D11_COMPARISON_LESS;
}

D3D11_DEPTH_WRITE_MASK TranslateDepthWriteMask( const String& key )
{
	if ( key == _T( "D3D11_DEPTH_WRITE_MASK_ZERO" ) )
	{
		return D3D11_DEPTH_WRITE_MASK_ZERO;
	}
	else if ( key == _T( "D3D11_DEPTH_WRITE_MASK_ALL" ) )
	{
		return D3D11_DEPTH_WRITE_MASK_ALL;
	}

	return D3D11_DEPTH_WRITE_MASK_ALL;
}

D3D11_STENCIL_OP TranslateStencilOP( const String& key )
{
	if ( key == _T( "D3D11_STENCIL_OP_KEEP" ) )
	{
		return D3D11_STENCIL_OP_KEEP;
	}
	else if ( key == _T( "D3D11_STENCIL_OP_ZERO" ) )
	{
		return D3D11_STENCIL_OP_ZERO;
	}
	else if ( key == _T( "D3D11_STENCIL_OP_REPLACE" ) )
	{
		return D3D11_STENCIL_OP_REPLACE;
	}
	else if ( key == _T( "D3D11_STENCIL_OP_INCR_SAT" ) )
	{
		return D3D11_STENCIL_OP_INCR_SAT;
	}
	else if ( key == _T( "D3D11_STENCIL_OP_DECR_SAT" ) )
	{
		return D3D11_STENCIL_OP_DECR_SAT;
	}
	else if ( key == _T( "D3D11_STENCIL_OP_INVERT" ) )
	{
		return D3D11_STENCIL_OP_INVERT;
	}
	else if ( key == _T( "D3D11_STENCIL_OP_INCR" ) )
	{
		return D3D11_STENCIL_OP_INCR;
	}
	else if ( key == _T( "D3D11_STENCIL_OP_DECR" ) )
	{
		return D3D11_STENCIL_OP_DECR;
	}
	return D3D11_STENCIL_OP_KEEP;
}

D3D11_FILTER TranslateFilter( const String& key )
{
	if ( key == _T( "D3D11_FILTER_MIN_MAG_MIP_POINT" ) )
	{
		return D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT" ) )
	{
		return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT" ) )
	{
		return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT" ) )
	{
		return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_MIN_MAG_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_ANISOTROPIC" ) )
	{
		return D3D11_FILTER_ANISOTROPIC;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR" ) )
	{
		return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	}
	else if ( key == _T( "D3D11_FILTER_COMPARISON_ANISOTROPIC" ) )
	{
		return D3D11_FILTER_COMPARISON_ANISOTROPIC;
	}

	return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
}

D3D11_TEXTURE_ADDRESS_MODE  TranslateTextrueAddressMode( const String& key )
{
	if ( key == _T( "D3D11_TEXTURE_ADDRESS_WRAP" ) )
	{
		return D3D11_TEXTURE_ADDRESS_WRAP;
	}
	else if ( key == _T( "D3D11_TEXTURE_ADDRESS_MIRROR" ) )
	{
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	}
	else if ( key == _T( "D3D11_TEXTURE_ADDRESS_CLAMP" ) )
	{
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	else if ( key == _T( "D3D11_TEXTURE_ADDRESS_BORDER" ) )
	{
		return D3D11_TEXTURE_ADDRESS_BORDER;
	}
	else if ( key == _T( "D3D11_TEXTURE_ADDRESS_MIRROR_ONCE" ) )
	{
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	}

	return D3D11_TEXTURE_ADDRESS_CLAMP;
}