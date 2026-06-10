#pragma once

#include "D3D12BindlessManager.h"
#include "D3D12DescriptorHeapAllocator.h"
#include "PipelineState.h"

#include <d3d12.h>

namespace agl
{
	class D3D12SamplerState final : public SamplerState
	{
	public:
		virtual int32 GetBindlessHandle() const override;

		const D3D12CpuDescriptorHandle& GetCpuHandle() const;

		D3D12SamplerState( const SamplerStateDesc& desc );
		D3D12SamplerState( const D3D12SamplerState& ) = default;
		D3D12SamplerState( D3D12SamplerState&& ) = default;
		D3D12SamplerState& operator=( const D3D12SamplerState& ) = default;
		D3D12SamplerState& operator=( D3D12SamplerState&& ) = default;
		virtual ~D3D12SamplerState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D12ViewDescriptorHandle m_descriptor;

		D3D12_SAMPLER_DESC m_d3dDesc;

		int32 m_bindlessHandle = NullBindlessHandle;
	};
}
