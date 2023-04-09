#pragma once

#include "SizedTypes.h"

#include <atomic>

class RefCounter
{
public:
	int32 AddRef()
	{
		++m_refCount;
		return m_refCount;
	}

	int32 ReleaseRef()
	{
		--m_refCount;

		int32 refCount = m_refCount;
		if ( refCount == 0 )
		{
			Finalizer();
		}

		return refCount;
	}

	int32 GetRefCount() const
	{
		return m_refCount;
	}

	virtual ~RefCounter() = default;

private:
	virtual void Finalizer() {}

	std::atomic<int32> m_refCount = 0;
};