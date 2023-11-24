#pragma once

#include "D3D12Texture.h"
#include "Memory/InlineMemoryAllocator.h"
#include "Viewport.h"

#include <dxgi1_6.h>
#include <vector>
#include <wrl/client.h>

namespace agl
{
	template <AglType Backend>
	class DxgiSwapchain;

	class D3D12Viewport final : public Viewport
	{
	public:
		virtual void Clear( const float( &clearColor )[4] ) override;
		virtual void Bind( ICommandListBase& commandList ) const override;

		virtual std::pair<uint32, uint32> Size() const override;
		virtual std::pair<uint32, uint32> SizeOnRenderThread() const override;
		virtual void Resize( const std::pair<uint32, uint32>& newSize ) override;
		virtual agl::Texture* Texture() override;

		D3D12Viewport( uint32 width, uint32 height, DXGI_FORMAT format, const float4& bgColor );
		D3D12Viewport( DxgiSwapchain<AglType::D3D12>& swapchain );

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		void CreateDedicateTexture();

		ViewportProxy m_proxy;

		uint32 m_width;
		uint32 m_height;
		DXGI_FORMAT m_format;
		float4 m_clearColor;

		uint32 m_bufferIndex = 0;

		RefHandle<D3D12Texture2D> m_frameBuffer;
		RefHandle<DxgiSwapchain<AglType::D3D12>> m_swapchain;
	};
}
