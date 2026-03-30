#include "GraphicsApiResource.h"

#include "HashUtil.h"

namespace agl
{
	void GraphicsApiResource::Finalizer()
	{
		Free();
		delete this;
	}

	unsigned char ConvertToColorWriteEnable( ColorWriteEnable colorWriteEnable )
	{
		return static_cast<unsigned char>( colorWriteEnable );
	}

	size_t BufferDesc::GetHash() const
	{
		static size_t typeHash = typeid( BufferDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_stride );
		HashCombine( hash, m_count );
		HashCombine( hash, m_access );
		HashCombine( hash, m_bindType );
		HashCombine( hash, m_miscFlag );
		HashCombine( hash, m_format );

		return hash;
	}

	size_t TextureDesc::GetHash() const
	{
		static size_t typeHash = typeid( TextureDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_width );
		HashCombine( hash, m_height );
		HashCombine( hash, m_depth );
		HashCombine( hash, m_sampleCount );
		HashCombine( hash, m_sampleQuality );
		HashCombine( hash, m_mipLevels );
		HashCombine( hash, m_format );
		HashCombine( hash, m_access );
		HashCombine( hash, m_bindType );
		HashCombine( hash, m_miscFlag );

		return hash;
	}

	size_t RasterizerStateDesc::GetHash() const
	{
		static size_t typeHash = typeid( RasterizerStateDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_fillMode );
		HashCombine( hash, m_cullMode );
		HashCombine( hash, m_frontCounterClockwise );
		HashCombine( hash, m_depthBias );
		HashCombine( hash, m_depthBiasClamp );
		HashCombine( hash, m_slopeScaleDepthBias );
		HashCombine( hash, m_depthClipEnable );
		HashCombine( hash, m_scissorEnable );
		HashCombine( hash, m_multisampleEnable );
		HashCombine( hash, m_antialiasedLineEnable );

		return hash;
	}

	size_t RenderTargetBlendDesc::GetHash() const
	{
		static size_t typeHash = typeid( RenderTargetBlendDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_blendEnable );
		HashCombine( hash, m_srcBlend );
		HashCombine( hash, m_destBlend );
		HashCombine( hash, m_blendOp );
		HashCombine( hash, m_srcBlendAlpha );
		HashCombine( hash, m_destBlendAlpha );
		HashCombine( hash, m_blendOpAlpha );
		HashCombine( hash, m_renderTargetWriteMask );

		return hash;
	}

	size_t BlendStateDesc::GetHash() const
	{
		static size_t typeHash = typeid( BlendStateDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_alphaToConverageEnable );
		HashCombine( hash, m_independentBlendEnable );

		for ( int32 i = 0; i < MaxRendertagets; ++i )
		{
			HashCombine( hash, m_renderTarget[i].GetHash() );
		}

		HashCombine( hash, m_sampleMask );

		return hash;
	}

	size_t StencilOpDesc::GetHash() const
	{
		static size_t typeHash = typeid( StencilOpDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_failOp );
		HashCombine( hash, m_depthFailOp );
		HashCombine( hash, m_passOp );
		HashCombine( hash, m_func );

		return hash;
	}

	size_t DepthStencilStateDesc::GetHash() const
	{
		static size_t typeHash = typeid( DepthStencilStateDesc ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_depthEnable );
		HashCombine( hash, m_depthWriteMode );
		HashCombine( hash, m_depthFunc );
		HashCombine( hash, m_stencilEnable );
		HashCombine( hash, m_stencilReadMask );
		HashCombine( hash, m_stencilWriteMask );
		HashCombine( hash, m_frontFace.GetHash() );
		HashCombine( hash, m_backFace.GetHash() );

		return hash;
	}

	size_t VertexLayoutData::GetHash() const
	{
		static size_t typeHash = typeid( VertexLayoutData ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_isInstanceData );
		HashCombine( hash, m_index );
		HashCombine( hash, m_format );
		HashCombine( hash, m_slot );
		HashCombine( hash, m_instanceDataStep );
		HashCombine( hash, m_name.Str() );

		return hash;
	}
}
