#pragma once

#include "Buffer.h"
#include "SizedTypes.h"

#include <d3d11.h>

namespace agl
{
	class D3D11Buffer final : public Buffer
	{
	public:
		ID3D11Buffer* Resource();
		virtual void* Resource() const override;

		const D3D11_BUFFER_DESC& GetDesc() const;

		uint32 Stride() const;

		D3D11Buffer( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData );
		virtual ~D3D11Buffer() override;
		D3D11Buffer( const D3D11Buffer& ) = delete;
		D3D11Buffer& operator=( const D3D11Buffer& ) = delete;
		D3D11Buffer( D3D11Buffer&& ) = delete;
		D3D11Buffer& operator=( D3D11Buffer&& ) = delete;

	protected:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		void CreateBuffer();
		void DestroyBuffer();

		ID3D11Buffer* m_buffer = nullptr;
		D3D11_BUFFER_DESC m_desc = {};
		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
		D3D11_SUBRESOURCE_DATA m_initData = {};
		void* m_dataStorage = nullptr;
		bool m_hasInitData = false;
	};
}
