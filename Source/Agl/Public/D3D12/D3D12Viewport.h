#pragma once

#include "Viewport.h"

#include <dxgi.h>
#include <wrl/client.h>

namespace agl
{
	// class D3D12BaseTexture2D;

	class D3D12Viewport : public Viewport
	{
	public:
		virtual DeviceError Present( bool vSync = false ) override;
		virtual void Clear( const float( &clearColor )[4] ) override;
		virtual void Bind( ICommandList& commandList ) const override;

		virtual void* Handle() const override
		{
			return m_hWnd;
		}

		virtual std::pair<uint32, uint32> Size() const override;

		virtual void Resize( const std::pair<uint32, uint32>& newSize ) override;

		virtual agl::Texture* Texture() override;

		D3D12Viewport( uint32 width, uint32 height, void* hWnd, DXGI_FORMAT format );

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		uint32 m_width;
		uint32 m_height;
		void* m_hWnd;
		DXGI_FORMAT m_format;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;

		// RefHandle<D3D11BaseTexture2D> m_backBuffer = nullptr;
	};
}
