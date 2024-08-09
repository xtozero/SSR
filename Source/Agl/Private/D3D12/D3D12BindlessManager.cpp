#include "D3D12BindlessManager.h"

#include "D3D12Api.h"
#include "DefaultAglConfig.h"

#include <cassert>

namespace agl
{
	int32 D3D12BindlessDescriptorHeap::Add( D3D12CpuDescriptorHandle handle )
	{
		if ( m_size >= m_capacity )
		{
			Grow();
		}

		auto freeIdx = static_cast<int32>( m_freeFlag.FindFirstSetBit() );
		assert( freeIdx != m_freeFlag.Size() );

		D3D12Device().CopyDescriptorsSimple( 1, m_cpuHeap.GetCpuHandle().At( freeIdx ), handle.At(), m_type );
		D3D12Device().CopyDescriptorsSimple( 1, m_gpuHeap.GetCpuHandle().At( freeIdx ), handle.At(), m_type );

		m_freeFlag[freeIdx] = false;
		++m_size;

		return freeIdx;
	}

	void D3D12BindlessDescriptorHeap::Remove( int32 bindlessHandle )
	{
		if ( bindlessHandle != NullBindlessHandle )
		{
			m_freeFlag[bindlessHandle] = true;
			--m_size;
		}
	}

	D3D12DescriptorHeap& D3D12BindlessDescriptorHeap::GetCpuHeap()
	{
		return m_cpuHeap;
	}

	const D3D12DescriptorHeap& D3D12BindlessDescriptorHeap::GetCpuHeap() const
	{
		return m_cpuHeap;
	}

	uint32 D3D12BindlessDescriptorHeap::GetCapacity() const
	{
		return m_capacity;
	}

	D3D12BindlessDescriptorHeap::D3D12BindlessDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE type )
		: m_type( type )
	{
	}

	void D3D12BindlessDescriptorHeap::Grow()
	{
		auto newCapacity = static_cast<uint32>( m_capacity * 1.5f + 1 );
		auto newCpuHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( m_type, newCapacity );
		auto newGpuHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocGpuDescriptorHeap( m_type, newCapacity );

		if ( m_capacity > 0 )
		{
			D3D12Device().CopyDescriptorsSimple( m_capacity, newCpuHeap.GetCpuHandle().At(), m_cpuHeap.GetCpuHandle().At(), m_type );
			D3D12Device().CopyDescriptorsSimple( m_capacity, newGpuHeap.GetCpuHandle().At(), m_cpuHeap.GetCpuHandle().At(), m_type );
		}

		m_capacity = newCapacity;
		m_cpuHeap = newCpuHeap;
		m_gpuHeap = newGpuHeap;
		m_freeFlag.Resize( m_capacity, true );
	}

	int32 D3D12BindlessManager::AddResourceDescriptor( D3D12CpuDescriptorHandle handle )
	{
		if ( DefaultAgl::IsSupportsBindless() == false )
		{
			return -1;
		}

		return m_resourceDescriptorHeap.Add( handle );
	}

	void D3D12BindlessManager::RemoveResourceDescriptor( int32 bindlessHandle )
	{
		if ( DefaultAgl::IsSupportsBindless() == false )
		{
			return;
		}

		m_resourceDescriptorHeap.Remove( bindlessHandle );
	}

	int32 D3D12BindlessManager::AddSamplerDescriptor( D3D12CpuDescriptorHandle handle )
	{
		if ( DefaultAgl::IsSupportsBindless() == false )
		{
			return -1;
		}

		return m_samplerDescriptorHeap.Add( handle );
	}

	void D3D12BindlessManager::RemoveSamplerDescriptor( int32 bindlessHandle )
	{
		if ( DefaultAgl::IsSupportsBindless() == false )
		{
			return;
		}

		m_samplerDescriptorHeap.Remove( bindlessHandle );
	}

	D3D12DescriptorHeap& D3D12BindlessManager::GetResourceCpuHeap()
	{
		return m_resourceDescriptorHeap.GetCpuHeap();
	}

	const D3D12DescriptorHeap& D3D12BindlessManager::GetResourceCpuHeap() const
	{
		return m_resourceDescriptorHeap.GetCpuHeap();
	}

	uint32 D3D12BindlessManager::GetHeapCapacity() const
	{
		return m_resourceDescriptorHeap.GetCapacity();
	}

	D3D12DescriptorHeap& D3D12BindlessManager::GetSamplerCpuHeap()
	{
		return m_samplerDescriptorHeap.GetCpuHeap();
	}

	const D3D12DescriptorHeap& D3D12BindlessManager::GetSamplerCpuHeap() const
	{
		return m_samplerDescriptorHeap.GetCpuHeap();
	}

	uint32 D3D12BindlessManager::GetSamplerHeapCapacity() const
	{
		return m_samplerDescriptorHeap.GetCapacity();
	}

	D3D12BindlessManager::D3D12BindlessManager() 
		: m_resourceDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV )
		, m_samplerDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER )
	{
	}
}
