#pragma once

#include "D3D12DescriptorHeapAllocator.h"
#include "BitArray.h"

namespace agl
{
	constexpr int32 NullBindlessHandle = -1;

	class D3D12BindlessDescriptorHeap
	{
	public:
		[[nodiscard]] int32 Add( D3D12CpuDescriptorHandle handle );
		void Remove( int32 bindlessHandle );

		D3D12DescriptorHeap& GetHeap();
		const D3D12DescriptorHeap& GetHeap() const;

		explicit D3D12BindlessDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type );

	private:
		void Grow();

		D3D12_DESCRIPTOR_HEAP_TYPE m_type;

		D3D12DescriptorHeap m_cpuHeap;
		D3D12DescriptorHeap m_gpuHeap;

		uint32 m_size = 0;
		uint32 m_capacity = 0;

		BitArray m_freeFlag;
	};

	class D3D12BindlessManager
	{
	public:
		[[nodiscard]] int32 AddDescriptor( D3D12CpuDescriptorHandle handle );
		void RemoveDescriptor( int32 bindlessHandle );

		[[nodiscard]] int32 AddSamplerDescriptor( D3D12CpuDescriptorHandle handle );
		void RemoveSamplerDescriptor( int32 bindlessHandle );

		D3D12DescriptorHeap& GetHeap();
		const D3D12DescriptorHeap& GetHeap() const;

		D3D12DescriptorHeap& GetSamperHeap();
		const D3D12DescriptorHeap& GetSamplerHeap() const;

		D3D12BindlessManager();

	private:
		D3D12BindlessDescriptorHeap m_descriptorHeap;
		D3D12BindlessDescriptorHeap m_samplerDescriptorHeap;
	};
}
