#pragma once

#include "GraphicsApiResource.h"
#include "Viewport.h"

#include <DXGI.h>
#include <wrl/client.h>

namespace aga
{
	class D3D11BaseTexture2D;

	class D3D11Viewport : public Viewport
	{
	public:
		virtual void Free( ) override;
		virtual void InitResource( ) override;

		virtual DEVICE_ERROR Present( bool vSync = false ) override;
		virtual void Clear( const float (&clearColor)[4] ) override;

		virtual std::pair<UINT, UINT> Size( ) const override;

		D3D11Viewport( int width, int height, HWND hWnd, DXGI_FORMAT format );

	private:
		UINT m_width;
		UINT m_height;
		DXGI_FORMAT m_format;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;

		RefHandle<D3D11BaseTexture2D> m_backBuffer = nullptr;
	};
}