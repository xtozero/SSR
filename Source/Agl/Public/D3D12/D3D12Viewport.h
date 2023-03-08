#pragma once

#include "D3D12BaseTexture.h"
#include "Memory/InlineMemoryAllocator.h"
#include "Viewport.h"

#include <dxgi1_6.h>
#include <vector>
#include <wrl/client.h>

namespace agl
{
	class D3D12Viewport : public Viewport
	{
	public:
		virtual void OnBeginFrameRendering() override;
		virtual void OnEndFrameRendering() override;
		virtual DeviceError Present( bool vSync = false ) override;
		virtual void Clear( const float( &clearColor )[4] ) override;
		virtual void Bind( ICommandListBase& commandList ) const override;

		virtual void* Handle() const override
		{
			return m_hWnd;
		}

		virtual std::pair<uint32, uint32> Size() const override;
		virtual void Resize( const std::pair<uint32, uint32>& newSize ) override;
		virtual agl::Texture* Texture() override;

		D3D12Viewport( uint32 width, uint32 height, uint32 bufferCount, void* hWnd, DXGI_FORMAT format );

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		uint32 m_width;
		uint32 m_height;
		uint32 m_bufferCount;
		void* m_hWnd;
		DXGI_FORMAT m_format;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pSwapChain;

		uint32 m_bufferIndex = 0;
		std::vector<RefHandle<D3D12BaseTexture2D>, InlineAllocator<RefHandle<D3D12BaseTexture2D>, 2>> m_backBuffers;
	};
}
