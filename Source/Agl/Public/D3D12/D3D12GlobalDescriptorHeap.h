#pragma once

#include "D3D12DescriptorHeapAllocator.h"
#include "SizedTypes.h"

#include <d3d12.h>
#include <deque>
#include <vector>
#include <wrl/client.h>

namespace agl
{
	class D3D12GlobalHeapAllocatedInfo final
	{
	public:
		ID3D12DescriptorHeap* GetDescriptorHeap();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle( uint32 offset = 0 ) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle( uint32 offset = 0 ) const;

		D3D12GlobalHeapAllocatedInfo( const D3D12DescriptorHeap& base, uint32 offset, uint32 end ) noexcept;
		D3D12GlobalHeapAllocatedInfo( const D3D12GlobalHeapAllocatedInfo& ) = default;
		D3D12GlobalHeapAllocatedInfo& operator=( const D3D12GlobalHeapAllocatedInfo& ) = default;
		D3D12GlobalHeapAllocatedInfo( D3D12GlobalHeapAllocatedInfo&& ) = default;
		D3D12GlobalHeapAllocatedInfo& operator=( D3D12GlobalHeapAllocatedInfo&& ) = default;

	private:
		D3D12DescriptorHeap m_base;
		uint32 m_offset = 0;
		uint32 m_end = 0;
	};

	class D3D12GlobalDescriptorHeap final
	{
	public:
		void Initialize();

		void Prepare();

		void OnEndFrameRendering();

		D3D12GlobalHeapAllocatedInfo GetHeapStart( D3D12_DESCRIPTOR_HEAP_TYPE type ) const;

		void UpdateBindless( D3D12_DESCRIPTOR_HEAP_TYPE type, int32 bindlessHandle, const D3D12CpuDescriptorHandle& handle );
		D3D12GlobalHeapAllocatedInfo Acquire( D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 num );

		D3D12GlobalDescriptorHeap() = default;
		~D3D12GlobalDescriptorHeap() = default;
		D3D12GlobalDescriptorHeap( const D3D12GlobalDescriptorHeap& ) = delete;
		D3D12GlobalDescriptorHeap& operator=( const D3D12GlobalDescriptorHeap& ) = delete;
		D3D12GlobalDescriptorHeap( D3D12GlobalDescriptorHeap&& ) = delete;
		D3D12GlobalDescriptorHeap& operator=( D3D12GlobalDescriptorHeap&& ) = delete;

	private:
		class RingAllocator final
		{
		public:
			void Initialize( uint32 begin, uint32 end );

			uint32 Acquire( uint32 num );

			void Prepare();

			void OnEndFrameRendering();

			RingAllocator() = default;
			~RingAllocator();
			RingAllocator( const RingAllocator& ) = delete;
			RingAllocator& operator=( const RingAllocator& ) = delete;
			RingAllocator( RingAllocator&& ) = delete;
			RingAllocator& operator=( RingAllocator&& ) = delete;

		private:
			struct FrameMarker
			{
				uint32 m_head = 0;
				uint64 m_fenceValue = 0;
			};

			uint32 m_begin = 0;
			uint32 m_end = 0;
			uint32 m_capacity = 0;

			uint32 m_head = 0;
			uint32 m_tail = 0;

			Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
			uint64 m_fenceValue = 0;
			HANDLE m_fenceEvent = nullptr;
			std::deque<FrameMarker> m_frameMarkers;
		};

		static constexpr uint32 NumDescriptorHeapType = 2;

		static constexpr uint32 MaxBindlessResources = 100000;
		static constexpr uint32 MaxFrameResources = 25000;

		static constexpr uint32 MaxBindlessSamplers = 1024;
		static constexpr uint32 MaxFrameSamplers = 256;

		static constexpr uint32 MaxBindlessDescriptorHeapSizes[NumDescriptorHeapType] = {
			MaxBindlessResources,
			MaxBindlessSamplers,
		};

		uint32 MaxDescriptorHeapSize[NumDescriptorHeapType] = {};

		D3D12DescriptorHeap m_heaps[NumDescriptorHeapType];
		RingAllocator m_ringAllocator[NumDescriptorHeapType];
	};
}