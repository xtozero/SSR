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
}
