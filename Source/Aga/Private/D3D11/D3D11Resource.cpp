#include "stdafx.h"
#include "D3D11/D3D11Resource.h"

#include "D3D11/Direct3D11.h"

#include <D3D11Shader.h>
#include <D3Dcompiler.h>
#include <D3DX11.h>

inline D3D11_BUFFER_DESC ConvertTraitToDesc( const BUFFER_TRAIT& trait )
{
	UINT byteWidth = trait.m_count * trait.m_stride;
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );
	UINT structureByteStride = trait.m_stride;

	return D3D11_BUFFER_DESC{ 
		byteWidth, 
		usage, 
		bindFlag, 
		cpuAccessFlag, 
		miscFlags, 
		structureByteStride 
	};
}

inline D3D11_TEXTURE1D_DESC ConvertTraitTo1DDesc( const TEXTURE_TRAIT& trait )
{
	DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

	return D3D11_TEXTURE1D_DESC{ 
		trait.m_width,
		trait.m_mipLevels,
		trait.m_depth,
		format,
		usage,
		bindFlag,
		cpuAccessFlag,
		miscFlags 
	};
}

inline D3D11_TEXTURE2D_DESC ConvertTraitTo2DDesc( const TEXTURE_TRAIT& trait )
{
	DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
	DXGI_SAMPLE_DESC SampleDesc = { trait.m_sampleCount, trait.m_sampleQuality };
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

	return D3D11_TEXTURE2D_DESC{ 
		trait.m_width,
		trait.m_height,
		trait.m_mipLevels,
		trait.m_depth,
		format,
		SampleDesc,
		usage,
		bindFlag,
		cpuAccessFlag,
		miscFlags 
	};
}

inline D3D11_TEXTURE3D_DESC ConvertTraitTo3DDesc( const TEXTURE_TRAIT& trait )
{
	DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

	return D3D11_TEXTURE3D_DESC{ 
		trait.m_width,
		trait.m_height,
		trait.m_depth,
		trait.m_mipLevels,
		format,
		usage,
		bindFlag,
		cpuAccessFlag,
		miscFlags
	};
}

inline TEXTURE_TRAIT Convert1DDescToTrait( const D3D11_TEXTURE1D_DESC& desc )
{
	UINT format = ConvertDxgiFormatToFormat( desc.Format );
	UINT type = ConvertBindToType( desc.BindFlags );
	UINT access = ConvertUsageToAccessFlag( desc.Usage );
	UINT misc = ConvertDXMiscToMisc( desc.MiscFlags );

	return TEXTURE_TRAIT{
		desc.Width,
		1,
		desc.ArraySize,
		0,
		0,
		desc.MipLevels,
		format,
		access,
		type,
		misc
	};
}

inline TEXTURE_TRAIT Convert2DDescToTrait( const D3D11_TEXTURE2D_DESC& desc )
{
	UINT format = ConvertDxgiFormatToFormat( desc.Format );
	UINT type = ConvertBindToType( desc.BindFlags );
	UINT access = ConvertUsageToAccessFlag( desc.Usage );
	UINT misc = ConvertDXMiscToMisc( desc.MiscFlags );

	return TEXTURE_TRAIT{ 
		desc.Width,
		desc.Height,
		desc.ArraySize,
		desc.SampleDesc.Count,
		desc.SampleDesc.Quality,
		desc.MipLevels,
		format,
		access,
		type,
		misc 
	};
}

inline TEXTURE_TRAIT Convert3DDescToTrait( const D3D11_TEXTURE3D_DESC& desc )
{
	UINT format = ConvertDxgiFormatToFormat( desc.Format );
	UINT type = ConvertBindToType( desc.BindFlags );
	UINT access = ConvertUsageToAccessFlag( desc.Usage );
	UINT misc = ConvertDXMiscToMisc( desc.MiscFlags );

	return TEXTURE_TRAIT{ 
		desc.Width,
		desc.Height,
		desc.Depth,
		0,
		0,
		desc.MipLevels,
		format,
		access,
		type,
		misc 
	};
}

inline D3D11_RENDER_TARGET_VIEW_DESC ConvertTraitToRTV( const TEXTURE_TRAIT& trait )
{
	D3D11_RENDER_TARGET_VIEW_DESC rtv = {};

	rtv.Format = ConvertFormatToDxgiFormat( trait.m_format );

	if ( IsTexture1D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			rtv.Texture1DArray.ArraySize = trait.m_depth;
		}
		else
		{
			rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
		}
	}
	else if ( IsTexture2D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			if ( trait.m_sampleCount > 1 )
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
				rtv.Texture2DMSArray.ArraySize = trait.m_depth;
			}
			else
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv.Texture2DArray.ArraySize = trait.m_depth;
			}
		}
		else
		{
			if ( trait.m_sampleCount > 1 )
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			}
		}
	}
	else if ( IsTexture3D( trait ) )
	{
		rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		rtv.Texture3D.WSize = trait.m_depth;
	}
	else
	{
		__debugbreak( );
	}

	return rtv;
}

inline D3D11_RENDER_TARGET_VIEW_DESC ConvertDescToRTV( const D3D11_TEXTURE2D_DESC& desc )
{
	D3D11_RENDER_TARGET_VIEW_DESC rtv = {};

	rtv.Format = desc.Format;

	if ( desc.ArraySize > 1 )
	{
		if ( desc.SampleDesc.Count > 1 )
		{
			rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtv.Texture2DMSArray.ArraySize = desc.ArraySize;
		}
		else
		{
			rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtv.Texture2DArray.ArraySize = desc.ArraySize;
		}
	}
	else
	{
		if ( desc.SampleDesc.Count > 1 )
		{
			rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		}
	}

	return rtv;
}

inline D3D11_DEPTH_STENCIL_VIEW_DESC ConvertTraitToDSV( const TEXTURE_TRAIT& trait )
{
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};

	dsv.Format = ConvertFormatToDxgiFormat( trait.m_format );

	if ( IsTexture1D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
			dsv.Texture1DArray.ArraySize = trait.m_depth;
		}
		else
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
		}
	}
	else if ( IsTexture2D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			if ( trait.m_sampleCount > 1 )
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
				dsv.Texture2DMSArray.ArraySize = trait.m_depth;
			}
			else
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				dsv.Texture2DArray.ArraySize = trait.m_depth;
			}
		}
		else
		{
			if ( trait.m_sampleCount > 1 )
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
		}
	}
	else
	{
		__debugbreak( );
	}

	return dsv;
}

inline D3D11_DEPTH_STENCIL_VIEW_DESC ConvertTraitToDSV( const D3D11_TEXTURE2D_DESC& desc )
{
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};

	dsv.Format = desc.Format;

	if ( desc.ArraySize > 1 )
	{
		if ( desc.SampleDesc.Count > 1 )
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsv.Texture2DMSArray.ArraySize = desc.ArraySize;
		}
		else
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsv.Texture2DArray.ArraySize = desc.ArraySize;
		}
	}
	else
	{
		if ( desc.SampleDesc.Count > 1 )
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		}
	}

	return dsv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertTraitToSRV( const TEXTURE_TRAIT& trait )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	srv.Format = ConvertFormatToDxgiFormat( trait.m_format );

	if ( IsTexture1D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
			srv.Texture1DArray.ArraySize = trait.m_depth;
			srv.Texture1DArray.MipLevels = trait.m_mipLevels;
		}
		else
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
			srv.Texture1D.MipLevels = trait.m_mipLevels;
		}
	}
	else if ( IsTexture2D( trait ) )
	{
		if ( trait.m_depth % 6 == 0 )
		{
			if ( trait.m_depth == 6 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
				srv.TextureCube.MipLevels = trait.m_mipLevels;
			}
			else
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
				srv.TextureCubeArray.MipLevels = trait.m_mipLevels;
				srv.TextureCubeArray.NumCubes = trait.m_depth / 6;
			}
		}
		else if ( trait.m_depth > 1 )
		{
			if ( trait.m_sampleCount > 1 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
				srv.Texture2DMSArray.ArraySize = trait.m_depth;
			}
			else
			{

				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
				srv.Texture2DArray.MipLevels = trait.m_mipLevels;
				srv.Texture2DArray.ArraySize = trait.m_depth;
			}
		}
		else
		{
			if ( trait.m_sampleCount > 1 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
				srv.Texture2D.MipLevels = trait.m_mipLevels;
			}
		}
	}
	else if ( IsTexture3D( trait ) )
	{
		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		srv.Texture3D.MipLevels = trait.m_mipLevels;
	}
	else
	{
		__debugbreak( );
	}

	return srv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertTraitToSRV( const BUFFER_TRAIT& trait )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( trait.m_miscFlag == RESOURCE_MISC::BUFFER_ALLOW_RAW_VIEWS )
	{
		if ( ( ( trait.m_stride * trait.m_count ) % 4 ) != 0 )
		{
			__debugbreak( );
		}

		srv.Format = DXGI_FORMAT_R32_TYPELESS;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srv.BufferEx.NumElements = static_cast<UINT>( ( trait.m_stride * trait.m_count ) / 4 );
		srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	}
	else
	{
		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.NumElements = trait.m_count;
		srv.Buffer.ElementWidth = trait.m_stride;
	}

	return srv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE1D_DESC& desc )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	srv.Format = desc.Format;

	if ( desc.ArraySize > 1 )
	{
		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
		srv.Texture1DArray.ArraySize = desc.ArraySize;
		srv.Texture1DArray.MipLevels = desc.MipLevels;
	}
	else
	{
		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
		srv.Texture1D.MipLevels = desc.MipLevels;
	}

	return srv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE2D_DESC& desc )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	srv.Format = desc.Format;

	if ( desc.ArraySize % 6 == 0 )
	{
		if ( desc.ArraySize == 6 )
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
			srv.TextureCube.MipLevels = desc.MipLevels;
		}
		else
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
			srv.TextureCubeArray.MipLevels = desc.MipLevels;
			srv.TextureCubeArray.NumCubes = desc.ArraySize / 6;
		}
	}
	else if ( desc.ArraySize > 1 )
	{
		if ( desc.SampleDesc.Count > 1 )
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srv.Texture2DMSArray.ArraySize = desc.ArraySize;
		}
		else
		{

			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
			srv.Texture2DArray.MipLevels = desc.MipLevels;
			srv.Texture2DArray.ArraySize = desc.ArraySize;
		}
	}
	else
	{
		if ( desc.SampleDesc.Count > 1 )
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
			srv.Texture2D.MipLevels = desc.MipLevels;
		}
	}

	return srv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE3D_DESC& desc )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	srv.Format = desc.Format;
	srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
	srv.Texture3D.MipLevels = desc.MipLevels;

	return srv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_BUFFER_DESC& desc )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( desc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
	{
		if ( ( desc.ByteWidth % 4 ) != 0 )
		{
			__debugbreak( );
		}

		srv.Format = DXGI_FORMAT_R32_TYPELESS;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srv.BufferEx.NumElements = desc.ByteWidth / 4;
		srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	}
	else
	{
		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
		srv.Buffer.ElementWidth = desc.StructureByteStride;
	}

	return srv;
}

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertTraitToUAV( const TEXTURE_TRAIT& trait )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	uav.Format = ConvertFormatToDxgiFormat( trait.m_format );

	if ( IsTexture1D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
			uav.Texture1DArray.ArraySize = trait.m_depth;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
		}
	}
	else if ( IsTexture2D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uav.Texture2DArray.ArraySize = trait.m_depth;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		}
	}
	else if ( IsTexture3D( trait ) )
	{
		__debugbreak( );
	}
	else
	{
		__debugbreak( );
	}

	return uav;
}

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertTraitToUAV( const BUFFER_TRAIT& trait )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	if ( trait.m_miscFlag == RESOURCE_MISC::BUFFER_ALLOW_RAW_VIEWS )
	{
		if ( ( ( trait.m_stride * trait.m_count ) % 4 ) != 0 )
		{
			__debugbreak( );
		}

		uav.Format = DXGI_FORMAT_R32_TYPELESS;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.NumElements = static_cast<UINT>( ( trait.m_stride * trait.m_count ) / 4 );
		uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	}
	else
	{
		uav.Format = DXGI_FORMAT_UNKNOWN;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.NumElements = trait.m_count;
	}

	return uav;
}

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_TEXTURE1D_DESC& desc )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	uav.Format = desc.Format;

	if ( desc.ArraySize > 1 )
	{
		uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
		uav.Texture1DArray.ArraySize = desc.ArraySize;
	}
	else
	{
		uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
	}

	return uav;
}

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_TEXTURE2D_DESC& desc )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	uav.Format = desc.Format;

	if ( desc.ArraySize > 1 )
	{
		uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uav.Texture2DArray.ArraySize = desc.ArraySize;
	}
	else
	{
		uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	}

	return uav;
}

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_BUFFER_DESC& desc )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	if ( desc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
	{
		if ( ( desc.ByteWidth % 4 ) != 0 )
		{
			__debugbreak( );
		}

		uav.Format = DXGI_FORMAT_R32_TYPELESS;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.NumElements = desc.ByteWidth / 4;
		uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	}
	else
	{
		uav.Format = DXGI_FORMAT_UNKNOWN;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
	}

	return uav;
}

inline D3D11_DEPTH_STENCIL_DESC ConvertTraitToDesc( const DEPTH_STENCIL_STATE_TRAIT& trait )
{
	return D3D11_DEPTH_STENCIL_DESC{
		trait.m_depthEnable,
		ConvertToDepthWriteMask( trait.m_depthWriteMode ),
		ConvertToComparisionFunc( trait.m_depthFunc ),
		trait.m_stencilEnable,
		trait.m_stencilReadMask,
		trait.m_stencilWriteMask,
		{
			ConvertToStencilOp( trait.m_frontFace.m_failOp ),
			ConvertToStencilOp( trait.m_frontFace.m_depthFailOp ),
			ConvertToStencilOp( trait.m_frontFace.m_passOp ),
			ConvertToComparisionFunc( trait.m_frontFace.m_func )
		},
		{
			ConvertToStencilOp( trait.m_backFace.m_failOp ),
			ConvertToStencilOp( trait.m_backFace.m_depthFailOp ),
			ConvertToStencilOp( trait.m_backFace.m_passOp ),
			ConvertToComparisionFunc( trait.m_backFace.m_func )
		}
	};
}

inline D3D11_RASTERIZER_DESC ConvertTraitToDesc( const RASTERIZER_STATE_TRAIT& trait )
{
	return D3D11_RASTERIZER_DESC{
		ConvertToFillMode( trait.m_fillMode ),
		ConvertToCullMode( trait.m_cullMode ),
		trait.m_frontCounterClockwise,
		trait.m_depthBias,
		trait.m_depthBiasClamp,
		trait.m_slopeScaleDepthBias,
		trait.m_depthClipEnable,
		trait.m_scissorEnable,
		trait.m_multisampleEnalbe,
		trait.m_antialiasedLineEnable
	};
}

inline D3D11_SAMPLER_DESC ConvertTraitToDesc( const SAMPLER_STATE_TRAIT& trait )
{
	return D3D11_SAMPLER_DESC{
		ConvertToFilter( trait.m_filter ),
		ConvertToTextureAddress( trait.m_addressU ),
		ConvertToTextureAddress( trait.m_addressV ),
		ConvertToTextureAddress( trait.m_addressW ),
		trait.m_mipLODBias,
		trait.m_maxAnisotropy,
		ConvertToComparisionFunc( trait.m_comparisonFunc ),
		{ 
			trait.m_borderColor[0], 
			trait.m_borderColor[1], 
			trait.m_borderColor[2], 
			trait.m_borderColor[3] 
		},
		trait.m_minLOD,
		trait.m_maxLOD
	};
}

inline D3D11_BLEND_DESC ConvertTraitToDesc( const BLEND_STATE_TRAIT& trait )
{
	D3D11_BLEND_DESC desc;

	desc.AlphaToCoverageEnable = trait.m_alphaToConverageEnable;
	desc.IndependentBlendEnable = trait.m_independentBlendEnable;

	for ( int i = 0; i < 8; ++i )
	{
		desc.RenderTarget[i].BlendEnable = trait.m_renderTarget[i].m_blendEnable;
		desc.RenderTarget[i].SrcBlend = ConvertToBlend( trait.m_renderTarget[i].m_srcBlend );
		desc.RenderTarget[i].DestBlend = ConvertToBlend( trait.m_renderTarget[i].m_destBlend );
		desc.RenderTarget[i].BlendOp = ConvertToBlendOp( trait.m_renderTarget[i].m_blendOp );
		desc.RenderTarget[i].SrcBlendAlpha = ConvertToBlend( trait.m_renderTarget[i].m_srcBlendAlpha );
		desc.RenderTarget[i].DestBlendAlpha = ConvertToBlend( trait.m_renderTarget[i].m_destBlendAlpha );
		desc.RenderTarget[i].BlendOpAlpha = ConvertToBlendOp( trait.m_renderTarget[i].m_blendOpAlpha );
		desc.RenderTarget[i].RenderTargetWriteMask = ConvertToColorWriteEnable( trait.m_renderTarget[i].m_renderTargetWriteMask );
	}

	return desc;
}

void CD3D11Buffer::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateBuffer( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Buffer::CD3D11Buffer( const BUFFER_TRAIT & trait, const RESOURCE_INIT_DATA* initData )
{
	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitToDesc( trait );
}

void CD3D11Texture1D::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateTexture1D( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Texture1D::CD3D11Texture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitTo1DDesc( trait );
}

void CD3D11Texture2D::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateTexture2D( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Texture2D::CD3D11Texture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	SetAppSizeDependency( ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT ) > 0 );

	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitTo2DDesc( trait );
}

void CD3D11Texture3D::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateTexture3D( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Texture3D::CD3D11Texture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitTo3DDesc( trait );
}


void CD3D11RenderTarget::InitResource( )
{
	ID3D11Resource* pResource = m_texture->Get( );
	assert( pResource );

	bool result = SUCCEEDED( D3D11Device( ).CreateRenderTargetView( pResource, &m_desc, m_pResource.GetAddressOf() ) );
	assert( result );
}

CD3D11RenderTarget::CD3D11RenderTarget( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_texture( texture )
{
	SetAppSizeDependency( m_texture->IsAppSizeDependency( ) );

	if ( traitOrNull )
	{
		m_desc = ConvertTraitToRTV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToRTV( texture->GetDesc( ) );
	}
}

void CD3D11DepthStencil::InitResource( )
{
	ID3D11Resource* pResource = m_texture->Get( );
	assert( pResource );

	bool result = SUCCEEDED( D3D11Device( ).CreateDepthStencilView( pResource, &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

void CD3D11DepthStencil::Clear( unsigned int clearFlag, float depth, unsigned char stencil )
{
	if ( m_pResource )
	{
		D3D11Context().ClearDepthStencilView( m_pResource.Get( ), clearFlag, depth, stencil );
	}
}

CD3D11DepthStencil::CD3D11DepthStencil( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_texture( texture )
{
	SetAppSizeDependency( m_texture->IsAppSizeDependency( ) );

	if ( traitOrNull )
	{
		m_desc = ConvertTraitToDSV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertTraitToDSV( m_texture->GetDesc( ) );
	}
}

void CD3D11ShaderResource::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device().CreateShaderResourceView( m_resource->Get( ), &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11ShaderResource::CD3D11ShaderResource( RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_resource( texture.Get( ) )
{
	if ( traitOrNull )
	{
		m_desc = ConvertTraitToSRV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToSRV( texture->GetDesc( ) );
	}
}

CD3D11ShaderResource::CD3D11ShaderResource( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_resource( texture.Get( ) )
{
	SetAppSizeDependency( texture->IsAppSizeDependency( ) );

	if ( traitOrNull )
	{
		m_desc = ConvertTraitToSRV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToSRV( texture->GetDesc( ) );
	}
}

CD3D11ShaderResource::CD3D11ShaderResource( RefHandle<CD3D11Texture3D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_resource( texture.Get( ) )
{
	if ( traitOrNull )
	{
		m_desc = ConvertTraitToSRV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToSRV( texture->GetDesc( ) );
	}
}

CD3D11ShaderResource::CD3D11ShaderResource( RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT* traitOrNull ) : m_resource( buffer.Get( ) )
{
	if ( traitOrNull )
	{
		m_desc = ConvertTraitToSRV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToSRV( buffer->GetDesc( ) );
	}
}

void CD3D11RandomAccessResource::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateUnorderedAccessView( m_resource->Get( ), &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11RandomAccessResource::CD3D11RandomAccessResource( RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT * traitOrNull ) : m_resource( texture.Get( ) )
{
	if ( traitOrNull )
	{
		m_desc = ConvertTraitToUAV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToUAV( texture->GetDesc( ) );
	}
}

CD3D11RandomAccessResource::CD3D11RandomAccessResource( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT * traitOrNull ) : m_resource( texture.Get( ) )
{
	SetAppSizeDependency( texture->IsAppSizeDependency( ) );

	if ( traitOrNull )
	{
		m_desc = ConvertTraitToUAV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToUAV( texture->GetDesc( ) );
	}
}

CD3D11RandomAccessResource::CD3D11RandomAccessResource( RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT * traitOrNull ) : m_resource( buffer.Get( ) )
{
	if ( traitOrNull )
	{
		m_desc = ConvertTraitToUAV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToUAV( buffer->GetDesc( ) );
	}
}

void CD3D11VertexLayout::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateInputLayout( m_imputDesc, m_layoutSize, m_vs->ByteCode( ), m_vs->ByteCodeSize( ), m_pResource.GetAddressOf() ) );
	assert( result );
}

CD3D11VertexLayout::CD3D11VertexLayout( RefHandle<CD3D11VertexShader>& vs, const VERTEX_LAYOUT* layout, int layoutSize ) : m_vs( vs ), m_layoutSize( layoutSize )
{
	m_imputDesc = new D3D11_INPUT_ELEMENT_DESC[m_layoutSize];
	ConvertVertexLayoutToInputLayout( m_imputDesc, layout, m_layoutSize );
}

CD3D11VertexLayout::~CD3D11VertexLayout( )
{
	delete[] m_imputDesc;
}

void CD3D11VertexShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateVertexShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

void CD3D11GeometryShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateGeometryShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

void CD3D11PixelShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreatePixelShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );

	assert( result );
}

void CD3D11ComputeShader::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateComputeShader( m_byteCodePtr, m_byteCodeSize, nullptr, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

void CD3D11DepthStencilState::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateDepthStencilState( &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11DepthStencilState::CD3D11DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

void CD3D11RasterizerState::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateRasterizerState( &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11RasterizerState::CD3D11RasterizerState( const RASTERIZER_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

void CD3D11SamplerState::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateSamplerState( &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11SamplerState::CD3D11SamplerState( const SAMPLER_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

void CD3D11BlendState::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateBlendState( &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11BlendState::CD3D11BlendState( const BLEND_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}
