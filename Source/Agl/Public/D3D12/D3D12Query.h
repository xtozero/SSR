#pragma once

#include "D3D12Buffer.h"
#include "Query.h"

#include <d3d12.h>
#include <vector>
#include <wrl/client.h>

namespace agl
{
	class D3D12QueryHeapBlock;

	struct D3D12Query final
	{
		D3D12_QUERY_TYPE m_type = D3D12_QUERY_TYPE_OCCLUSION;
		D3D12QueryHeapBlock* m_heap = nullptr;
		int32 m_offset = 0;
	};

	class D3D12QueryHeapBlock final
	{
	public:
		void InitResource( D3D12_QUERY_HEAP_TYPE type );

		D3D12Query Allocate( D3D12_QUERY_TYPE type );
		void Deallocate( const D3D12Query& query );

		bool CanAllocate() const;

		bool IsOwned( const D3D12Query& query ) const;

		ID3D12QueryHeap* GetHeap();
		ID3D12Resource* GetReadBackBuffer();

		void GetData( void* outData, int32 size, int32 offset );

		~D3D12QueryHeapBlock();

	private:
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> m_heap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_readBackBuffer;

		void* m_readBackPtr = nullptr;

		std::vector<int32> m_freeList;
		uint32 m_freeSize = 0;
	};

	class D3D12QueryHeapBudget final
	{
	public:
		D3D12Query Allocate( D3D12_QUERY_TYPE type );
		void Deallocate( const D3D12Query& query );

	private:
		D3D12QueryHeapBlock* FindProperBlock();

		std::vector<D3D12QueryHeapBlock> m_blocks;
	};

	class D3D12QueryAllocator final
	{
	public:
		D3D12Query Allocate( D3D12_QUERY_TYPE type );
		void Deallocate( const D3D12Query& query );

	private:
		uint32 GetHeapIndex( D3D12_QUERY_TYPE type );

		static const int32 MaxQueryHeapType = 6;

		D3D12QueryHeapBudget m_heaps[MaxQueryHeapType];
	};

	class D3D12GpuTimer final : public GpuTimer
	{
	public:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		virtual void Begin( ICommandListBase& commandList ) override;
		virtual void End( ICommandListBase& commandList ) override;

		virtual double GetDuration() override;

	private:
		D3D12Query m_timeStampBegin;
		D3D12Query m_timeStampEnd;
	};
}