#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

#include <array>
#include <d3d11.h>
#include <wrl/client.h>

class IDeviceDependant : public IGraphicsApiResource
{
public:
	virtual ~IDeviceDependant( ) = default;

	virtual ID3D11DeviceChild* Get( ) = 0;

	virtual void InitResource( ) = 0;
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

enum class TEXTURE_TYPE
{
	TEXTURE_NONE = -1,
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D
};
