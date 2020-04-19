#pragma once

#include "common.h"
#include "Resource.h"

#include <atomic>
#include <array>
#include <d3d11.h>
#include <wrl/client.h>

class IDeviceDependant
{
public:
	virtual ~IDeviceDependant( ) = default;

	virtual ID3D11DeviceChild* Get( ) = 0;
	virtual void Free( ) = 0;

	virtual void InitResource( ) = 0;

	int AddRef( )
	{
		++m_refCount;
		return m_refCount;
	}

	int ReleaseRef( )
	{
		--m_refCount;

		int refCount = m_refCount;
		if ( refCount == 0 )
		{
			Free( );
			delete this;
		}

		return refCount;
	}

private:
	std::atomic<int> m_refCount = 0;
};

class IGraphicsResource : public IDeviceDependant
{
public:
	virtual ID3D11Resource* Get( ) = 0;
};

class IGraphicsView : public IDeviceDependant
{
public:
	virtual ID3D11View* Get( ) = 0;
};

template <typename NativeType, typename BaseType>
class GraphicsAssetBase : public BaseType
{
public:
	virtual NativeType* Get( ) override
	{
		if ( m_needInitialize )
		{
			m_needInitialize = false;
			InitResource( );
		}

		return m_pResource.Get( );
	};

	virtual void Free( ) override
	{
		m_needInitialize = true;
		m_pResource.Reset( );
	};

protected:
	GraphicsAssetBase( ) = default;
	Microsoft::WRL::ComPtr<NativeType> m_pResource;

private:
	bool m_needInitialize = false;
};

template <typename NativeType>
using GraphicsResourceBase = GraphicsAssetBase<NativeType, IGraphicsResource>;

template <typename NativeType>
using GraphicsViewBase = GraphicsAssetBase<NativeType, IGraphicsView>;

template <typename NativeType>
using DeviceDependantBase = GraphicsAssetBase<NativeType, IDeviceDependant>;

class FrameBufferDependent
{
public:
	void SetAppSizeDependency( bool isDepentant ) { m_isAppSizeDependent = isDepentant; }
	bool IsAppSizeDependency( ) const { return m_isAppSizeDependent; }

protected:
	FrameBufferDependent( ) = default;

private:
	bool m_isAppSizeDependent = false;
};

template <typename ReferencedType>
class RefHandle
{
public:
	ReferencedType* Get( ) const
	{
		return m_reference;
	}

	RefHandle( ) = default;
	explicit RefHandle( ReferencedType* reference ) : m_reference( reference )
	{
		if ( m_reference )
		{
			m_reference->AddRef( );
		}
	}

	~RefHandle( )
	{
		if ( m_reference )
		{
			m_reference->ReleaseRef( );
		}
	}

	RefHandle( const RefHandle& ref )
	{
		*this = ref;
	}

	RefHandle& operator=( const RefHandle& other )
	{
		if ( this != &other )
		{
			m_reference = other.m_reference;
			if ( m_reference )
			{
				m_reference->AddRef( );
			}
		}

		return *this;
	}

	RefHandle( RefHandle&& other )
	{
		*this = std::move( other );
	}

	RefHandle& operator=( RefHandle&& other )
	{
		if ( this != &other )
		{
			if ( m_reference )
			{
				m_reference->ReleaseRef( );
			}

			m_reference = other.m_reference;
			other.m_reference = nullptr;
		}

		return *this;
	}

	ReferencedType* operator->( )
	{
		return m_reference;
	}

	friend bool operator==( const RefHandle& lhs, ReferencedType* rhs )
	{
		return lhs.m_reference == rhs;
	}

	friend bool operator==( const RefHandle& lhs, const RefHandle& rhs )
	{
		return lhs.m_reference == rhs.m_reference;
	}

private:
	ReferencedType* m_reference = nullptr;
};

enum class TEXTURE_TYPE
{
	TEXTURE_NONE = -1,
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D
};
