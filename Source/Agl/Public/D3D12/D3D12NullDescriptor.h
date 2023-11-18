#pragma once

#include "D3D12Api.h"
#include "D3D12DescriptorHeapAllocator.h"

#include <d3d12.h>

namespace agl
{
	enum class DescriptorType
	{
		ConstantBufferView,
		ShaderResourceView,
		UnorderedAccessView,
		SamplerView,
		RenderTargetView,
		DepthStencilView,
		Count
	};
	constexpr uint32 MaxDescriptorType = static_cast<uint32>( DescriptorType::Count );

	enum class DescriptorUnderlyingType
	{
		Buffer,
		Texture2D,
		Texture3D,
		Count
	};
	constexpr uint32 MaxDescriptorUnderlyingType = static_cast<uint32>( DescriptorUnderlyingType::Count );

	class D3D12NullDescriptorStorage final
	{
	public:
		static void Clear();
		static constexpr D3D12DescriptorHeap& GetNullDescriptorHeap( DescriptorType descType, DescriptorUnderlyingType underlyingType )
		{
			auto descTypeIndex = static_cast<uint32>( descType );
			auto underlyingTypeIndex = static_cast<uint32>( underlyingType );

			return m_nullDescriptors[descTypeIndex * MaxDescriptorUnderlyingType + underlyingTypeIndex];
		}

	private:
		static D3D12DescriptorHeap m_nullDescriptors[MaxDescriptorType * MaxDescriptorUnderlyingType];
	};

	template <DescriptorType descType, DescriptorUnderlyingType underlyingType>
	class D3D12NullDescriptor final
	{
	public:
		static D3D12DescriptorHeap Get()
		{
			CreateDescriptorIfNotExist();
			return D3D12NullDescriptorStorage::GetNullDescriptorHeap( descType, underlyingType );
		}

	private:
		static void CreateDescriptorIfNotExist()
		{
			D3D12DescriptorHeap& descriptorHeap = D3D12NullDescriptorStorage::GetNullDescriptorHeap( descType, underlyingType );

			if ( descriptorHeap.IsNull() == false )
			{
				return;
			}

			if constexpr ( descType == DescriptorType::RenderTargetView )
			{
				descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1 );

				D3D12_RENDER_TARGET_VIEW_DESC desc = {};
				if constexpr ( underlyingType == DescriptorUnderlyingType::Buffer )
				{
					desc.Format = DXGI_FORMAT_UNKNOWN;
					desc.ViewDimension = D3D12_RTV_DIMENSION_BUFFER;
				}
				else if constexpr ( underlyingType == DescriptorUnderlyingType::Texture2D )
				{
					desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
					desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				}
				else if constexpr ( underlyingType == DescriptorUnderlyingType::Texture3D )
				{
					desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
					desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
				}

				D3D12Device().CreateRenderTargetView( nullptr, &desc, descriptorHeap.GetCpuHandle().At() );
			}
		}
	};
}