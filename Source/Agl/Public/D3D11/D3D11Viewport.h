#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "Viewport.h"

#include <dxgi1_6.h>
#include <wrl/client.h>

namespace agl
{
	class D3D11BaseTexture2D;

	template <AglType Backend>
	class DxgiSwapchain;

	class D3D11Viewport : public Viewport
	{
	public:
		virtual void Clear( const float (&clearColor)[4] ) override;
		virtual void Bind( ICommandListBase& commandList ) const override;

		virtual std::pair<uint32, uint32> Size() const override;
		virtual std::pair<uint32, uint32> SizeOnRenderThread() const override;
		virtual void Resize( const std::pair<uint32, uint32>& newSize ) override;
		virtual agl::Texture* Texture() override;

		D3D11Viewport( uint32 width, uint32 height, DXGI_FORMAT format );
		D3D11Viewport( DxgiSwapchain<AglType::D3D11>& swapchain );

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		void CreateDedicateTexture();

		ViewportProxy m_proxy;

		uint32 m_width;
		uint32 m_height;
		DXGI_FORMAT m_format;

		RefHandle<D3D11BaseTexture2D> m_frameBuffer;
		RefHandle<DxgiSwapchain<AglType::D3D11>> m_swapchain;
	};
}