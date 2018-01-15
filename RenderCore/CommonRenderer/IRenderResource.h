#pragma once

#include "../common.h"
#include "Resource.h"

#include <array>

struct ID3D11RenderTargetView;

class IRenderResource
{
public:
	virtual void* Get( ) const = 0;

	virtual ~IRenderResource( ) = default;

	void SetAppSizeDependency( bool isDepentant ) { m_isAppSizeDependent = isDepentant; }
	bool IsAppSizeDependency() const { return m_isAppSizeDependent; }

protected:
	IRenderResource( ) = default;

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

class ITexture : public IRenderResource
{
public:
	virtual TEXTURE_TYPE GetType( ) const = 0;
	const TEXTURE_TRAIT& GetTrait( ) const { return m_trait; }

	virtual ~ITexture( ) = default;

protected:
	TEXTURE_TRAIT m_trait;
};

class RenderTargetBinder
{
public:
	ID3D11RenderTargetView* const* Get( ) const { return m_renderTargets.data( ); }
	void Bind( int index, ID3D11RenderTargetView* pRenderTarget )
	{
		m_renderTargets[index] = pRenderTarget;
		int size = index + 1;
		m_curSize = size > m_curSize ? size : m_curSize;
	}
	int Count( ) const { return m_curSize; }

	RenderTargetBinder( ) : m_curSize( 0 )
	{
		m_renderTargets.fill( nullptr );
	}
private:
	static const int MAX_MRT_COUNT = 8;

	std::array<ID3D11RenderTargetView*, MAX_MRT_COUNT> m_renderTargets;
	int m_curSize;
};