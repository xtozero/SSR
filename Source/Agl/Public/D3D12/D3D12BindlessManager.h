#pragma once

#include "D3D12DescriptorHeapAllocator.h"
#include "BitArray.h"

namespace agl
{
	constexpr int32 NullBindlessHandle = -1;

	class D3D12BindlessDescriptorHeap
	{
	public:
		void Prepare();

		[[nodiscard]] int32 Add( const D3D12CpuDescriptorHandle& handle );
		void Remove( int32 bindlessHandle );

		D3D12DescriptorHeap& GetCpuHeap();
		const D3D12DescriptorHeap& GetCpuHeap() const;

		uint32 GetCapacity() const;

		explicit D3D12BindlessDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type );

	private:
		void Grow();

		D3D12_DESCRIPTOR_HEAP_TYPE m_type;

		D3D12DescriptorHeap m_cpuHeap;

		uint32 m_size = 0;
		uint32 m_capacity = 0;

		BitArray m_freeFlag;

		struct PendingReleaseInfo
		{
			uint32 m_remainFrame = 0;
			int32 m_handle = NullBindlessHandle;
		};
		std::vector<PendingReleaseInfo> m_pendingRelease;
	};

	class D3D12BindlessManager
	{
	public:
		void Prepare();

		[[nodiscard]] int32 AddResourceDescriptor( const D3D12CpuDescriptorHandle& handle );
		void RemoveResourceDescriptor( int32 bindlessHandle );

		[[nodiscard]] int32 AddSamplerDescriptor( const D3D12CpuDescriptorHandle& handle );
		void RemoveSamplerDescriptor( int32 bindlessHandle );

		D3D12DescriptorHeap& GetResourceCpuHeap();
		const D3D12DescriptorHeap& GetResourceCpuHeap() const;
		
		uint32 GetHeapCapacity() const;

		D3D12DescriptorHeap& GetSamplerCpuHeap();
		const D3D12DescriptorHeap& GetSamplerCpuHeap() const;

		uint32 GetSamplerHeapCapacity() const;

		D3D12BindlessManager();

	private:
		D3D12BindlessDescriptorHeap m_resourceDescriptorHeap;
		D3D12BindlessDescriptorHeap m_samplerDescriptorHeap;
	};
}
