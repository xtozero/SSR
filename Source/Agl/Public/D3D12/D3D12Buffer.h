#pragma once

#include "Buffer.h"
#include "D3D12ResourceAllocator.h"
#include "D3D12ResourceViews.h"
#include "InlineMemoryAllocator.h"
#include "SizedTypes.h"

#include <d3d12.h>
#include <vector>

namespace agl
{
	class D3D12Buffer : public Buffer
	{
	public:
		ID3D12Resource* Resource();
		virtual void* Resource() const override;

		virtual uint32 CurFrameOffset() const;

		uint32 Stride() const;
		uint32 Size() const;

		const D3D12_RESOURCE_DESC& Desc() const;

		D3D12Buffer( const BufferTrait& trait, const void* initData );
		virtual ~D3D12Buffer() override;
		D3D12Buffer( const D3D12Buffer& ) = delete;
		D3D12Buffer& operator=( const D3D12Buffer& ) = delete;
		D3D12Buffer( D3D12Buffer&& ) = delete;
		D3D12Buffer& operator=( D3D12Buffer&& ) = delete;

	protected:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		virtual void CreateBuffer();
		virtual void DestroyBuffer();

		void* m_dataStorage = nullptr;
		bool m_hasInitData = false;

		AllocatedResourceInfo m_resourceInfo;
		D3D12_RESOURCE_DESC m_desc = {};
		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
	};

	class D3D12ConstantBuffer : public D3D12Buffer
	{
	public:
		virtual uint32 CurFrameOffset() const override;

		D3D12ConstantBufferView* CBV() const;

		D3D12ConstantBuffer( const BufferTrait& trait, const void* initData );
		virtual ~D3D12ConstantBuffer() override = default;
		D3D12ConstantBuffer( const D3D12ConstantBuffer& ) = delete;
		D3D12ConstantBuffer& operator=( const D3D12ConstantBuffer& ) = delete;
		D3D12ConstantBuffer( D3D12ConstantBuffer&& ) = delete;
		D3D12ConstantBuffer& operator=( D3D12ConstantBuffer&& ) = delete;

	protected:
		virtual void CreateBuffer();
		virtual void DestroyBuffer();

	private:
		std::vector<RefHandle<D3D12ConstantBufferView>, InlineAllocator<RefHandle<D3D12ConstantBufferView>, 2>> m_cbv;
	};

	class D3D12IndexBuffer : public D3D12Buffer
	{
	public:
		const D3D12_INDEX_BUFFER_VIEW& GetView() const;

		D3D12IndexBuffer( const BufferTrait& trait, const void* initData );
		virtual ~D3D12IndexBuffer() override = default;
		D3D12IndexBuffer( const D3D12IndexBuffer& ) = delete;
		D3D12IndexBuffer& operator=( const D3D12IndexBuffer& ) = delete;
		D3D12IndexBuffer( D3D12IndexBuffer&& ) = delete;
		D3D12IndexBuffer& operator=( D3D12IndexBuffer&& ) = delete;

	protected:
		virtual void CreateBuffer();

	private:
		D3D12_INDEX_BUFFER_VIEW m_view = {};
	};

	class D3D12VertexBuffer : public D3D12Buffer
	{
	public:
		const D3D12_VERTEX_BUFFER_VIEW& GetView() const;

		D3D12VertexBuffer( const BufferTrait& trait, const void* initData );
		virtual ~D3D12VertexBuffer() override = default;
		D3D12VertexBuffer( const D3D12VertexBuffer& ) = delete;
		D3D12VertexBuffer& operator=( const D3D12VertexBuffer& ) = delete;
		D3D12VertexBuffer( D3D12VertexBuffer&& ) = delete;
		D3D12VertexBuffer& operator=( D3D12VertexBuffer&& ) = delete;

	protected:
		virtual void CreateBuffer();

	private:
		D3D12_VERTEX_BUFFER_VIEW m_view = {};
	};
}
