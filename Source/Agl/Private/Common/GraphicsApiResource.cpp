#include "stdafx.h"
#include "GraphicsApiResource.h"

#include "HashUtil.h"

namespace agl
{
	int32 GraphicsApiResource::AddRef()
	{
		++m_refCount;
		return m_refCount;
	}

	int32 GraphicsApiResource::ReleaseRef()
	{
		--m_refCount;

		int32 refCount = m_refCount;
		if ( refCount == 0 )
		{
			Free();
			delete this;
		}

		return refCount;
	}

	int32 GraphicsApiResource::GetRefCount() const
	{
		return m_refCount;
	}

	size_t TEXTURE_TRAIT::GetHash() const
	{
		static size_t typeHash = typeid( TEXTURE_TRAIT ).hash_code();
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
