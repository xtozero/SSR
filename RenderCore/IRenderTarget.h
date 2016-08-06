#pragma once

#include "common.h"

#include <array>
#include <cassert>

struct ID3D11Device;
struct ID3D11RenderTargetView;
struct ID3D11Resource;
struct D3D11_RENDER_TARGET_VIEW_DESC;
struct ID3D11DeviceContext;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IRenderTarget
{
public:
	virtual bool CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc ) = 0;
	virtual ID3D11RenderTargetView* Get( ) const = 0;
	virtual void SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView ) = 0;

	virtual void Clear( ID3D11DeviceContext* pDeviceContext, const float (&clearColor)[4] ) = 0;

	virtual ~IRenderTarget( ) = default;

protected:
	IRenderTarget( ) = default;
};

class RenderTargetBinder
{
public:
	ID3D11RenderTargetView* const* Get( ) const { return m_renderTargets.data(); }
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