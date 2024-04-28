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

	size_t TextureTrait::GetHash() const
	{
		static size_t typeHash = typeid( TextureTrait ).hash_code();
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

	size_t RasterizerStateTrait::GetHash() const
	{
		static size_t typeHash = typeid( RasterizerStateTrait ).hash_code();
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

	size_t RenderTargetBlendTrait::GetHash() const
	{
		static size_t typeHash = typeid( RenderTargetBlendTrait ).hash_code();
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

	size_t BlendStateTrait::GetHash() const
	{
		static size_t typeHash = typeid( BlendStateTrait ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_alphaToConverageEnable );
		HashCombine( hash, m_independentBlendEnable );

		for ( int32 i = 0; i < MAX_RENDER_TARGET; ++i )
		{
			HashCombine( hash, m_renderTarget[i].GetHash() );
		}

		HashCombine( hash, m_sampleMask );

		return hash;
	}

	size_t StencilOpTrait::GetHash() const
	{
		static size_t typeHash = typeid( StencilOpTrait ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_failOp );
		HashCombine( hash, m_depthFailOp );
		HashCombine( hash, m_passOp );
		HashCombine( hash, m_func );

		return hash;
	}

	size_t DepthStencilStateTrait::GetHash() const
	{
		static size_t typeHash = typeid( DepthStencilStateTrait ).hash_code();
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

	size_t VertexLayoutTrait::GetHash() const
	{
		static size_t typeHash = typeid( VertexLayoutTrait ).hash_code();
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
