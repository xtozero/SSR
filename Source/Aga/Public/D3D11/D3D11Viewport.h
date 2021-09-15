#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "Viewport.h"

#include <dxgi.h>
#include <wrl/client.h>

namespace aga
{
	class D3D11BaseTexture2D;

	class D3D11Viewport : public Viewport
	{
	public:
		virtual DEVICE_ERROR Present( bool vSync = false ) override;
		virtual void Clear( const float (&clearColor)[4] ) override;
		virtual void Bind( ) override;

		virtual void* Handle( ) const override
		{
			return m_hWnd;
		}

		virtual std::pair<uint32, uint32> Size( ) const override;

		virtual void Resize( const std::pair<uint32, uint32>& newSize ) override;

		virtual aga::Texture* Texture( ) override;

		D3D11Viewport( uint32 width, uint32 height, void* hWnd, DXGI_FORMAT format );

	private:
		virtual void InitResource( ) override;
		virtual void FreeResource( ) override;

		uint32 m_width;
		uint32 m_height;
		void* m_hWnd;
		DXGI_FORMAT m_format;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;

		RefHandle<D3D11BaseTexture2D> m_backBuffer = nullptr;
	};
}