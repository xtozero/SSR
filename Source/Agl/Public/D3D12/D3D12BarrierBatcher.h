#pragma once

#include "SizedTypes.h"

#include <d3d12.h>
#include <vector>

namespace agl
{
	class D3D12CommandListImpl;

	struct ResourceTransition;

	class D3D12BarrierBatcher
	{
	public:
		void AddTransition( const ResourceTransition& transition );
		void AddUavBarrier( const UavBarrier& uavBarrier );

		void Commit( D3D12CommandListImpl& commandList );

		D3D12BarrierBatcher();

	private:
		static constexpr size_t DefaultCapacity = 16;

		std::vector<D3D12_RESOURCE_BARRIER> m_barriers;
	};
}