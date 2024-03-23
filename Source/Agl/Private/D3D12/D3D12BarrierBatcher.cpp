#include "D3D12BarrierBatcher.h"

#include "D3D12CommandList.h"
#include "D3D12FlagConvertor.h"

namespace agl
{
	void D3D12BarrierBatcher::AddTransition( const ResourceTransition& transition )
	{
		ResourceState beforeState = transition.m_pTransitionable->GetResourceState();
		ResourceState afterState = transition.m_state;

		if ( beforeState != afterState )
		{
			m_barriers.emplace_back( ConvertToResourceBarrier( transition ) );
			transition.m_pTransitionable->SetResourceState( transition.m_state );
		}
	}

	void D3D12BarrierBatcher::AddUavBarrier( const UavBarrier& uavBarrier )
	{
		D3D12_RESOURCE_BARRIER barrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.UAV = {
				.pResource = static_cast<ID3D12Resource*>( uavBarrier.m_pResource )
			}
		};

		m_barriers.emplace_back( barrier );
	}

	void D3D12BarrierBatcher::Commit( D3D12CommandListImpl& commandList )
	{
		if ( m_barriers.empty() )
		{
			return;
		}

		commandList.ResourceBarrier( static_cast<uint32>( m_barriers.size() ), m_barriers.data() );
		m_barriers.clear();
	}

	D3D12BarrierBatcher::D3D12BarrierBatcher()
	{
		m_barriers.reserve( DefaultCapacity );
	}
}