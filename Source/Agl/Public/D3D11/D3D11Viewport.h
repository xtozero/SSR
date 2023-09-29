#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "Viewport.h"

#include <dxgi1_6.h>
#include <wrl/client.h>

namespace agl
{
	class D3D11BaseTexture2D;

	class D3D11Viewport : public Viewport
	{
	public:
		virtual void OnBeginFrameRendering() override {};
		virtual void OnEndFrameRendering() override {};
		virtual DeviceError Present( bool vSync = false ) override;
		virtual void Clear( const float (&clearColor)[4] ) override;
		virtual void Bind( ICommandListBase& commandList ) const override;

		virtual void* Handle() const override
		{
			return m_hWnd;
		}

		virtual std::pair<uint32, uint32> Size() const override;
		virtual void Resize( const std::pair<uint32, uint32>& newSize ) override;
		virtual agl::Texture* Texture() override;
		virtual agl::Texture* Canvas() override;

		D3D11Viewport( uint32 width, uint32 height, void* hWnd, DXGI_FORMAT format, bool useDedicateTexture );

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		void CreateDedicateTexture();

		uint32 m_width;
		uint32 m_height;
		void* m_hWnd;
		DXGI_FORMAT m_format;
		bool m_useDedicateTexture;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;

		RefHandle<D3D11BaseTexture2D> m_backBuffer;

		RefHandle<D3D11BaseTexture2D> m_frameBuffer;
	};
}