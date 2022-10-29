#pragma once

#include "Buffer.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
	class D3D12Buffer : public Buffer
	{
	public:
		ID3D12Resource* Resource();
		const ID3D12Resource* Resource() const;

		uint32 Stride() const;

		D3D12Buffer( const BUFFER_TRAIT& trait, const void* initData );
		~D3D12Buffer();
		D3D12Buffer( const D3D12Buffer& ) = delete;
		D3D12Buffer& operator=( const D3D12Buffer& ) = delete;
		D3D12Buffer( D3D12Buffer&& ) = delete;
		D3D12Buffer& operator=( D3D12Buffer&& ) = delete;

	protected:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		void CreateBuffer();
		void DestroyBuffer();

		void* m_dataStorage = nullptr;
		bool m_hasInitData = false;
	};
}
