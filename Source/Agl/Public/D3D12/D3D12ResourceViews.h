#pragma once

#include "D3D12DescriptorHeapAllocator.h"
#include "ResourceViews.h"

#include <d3d12.h>

namespace agl
{
	template <typename BaseClass, typename DescType>
	class D3D12ViewBase : public BaseClass
	{
	public:
		ID3D12DescriptorHeap* Resource() 
		{ 
			return m_descriptorHeap.GetHeap();
		}

		const ID3D12DescriptorHeap* Resource() const 
		{ 
			return m_descriptorHeap.GetHeap();
		}

		const D3D12CpuDescriptorHandle& GetCpuHandle() const
		{
			return m_descriptorHeap.GetCpuHandle();
		}

		const D3D12GpuDescriptorHandle& GetGpuHandle() const
		{
			return m_descriptorHeap.GetGpuHandle();
		}

		const IResourceViews* ViewHolder() const { return m_viewHolder; }

		D3D12ViewBase( IResourceViews* viewHolder, ID3D12Resource* d3d11Resource, const DescType& desc ) noexcept :
			m_viewHolder( viewHolder ),
			m_d3d12Resource( d3d11Resource ),
			m_desc( desc )
		{
			if ( m_d3d12Resource )
			{
				m_d3d12Resource->AddRef();
			}
		}

		virtual ~D3D12ViewBase()
		{
			this->Free();
		}

		D3D12ViewBase( const D3D12ViewBase& other )
		{
			*this = other;
		}

		D3D12ViewBase& operator=( const D3D12ViewBase& other )
		{
			if ( this != &other )
			{
				if ( m_d3d12Resource )
				{
					m_d3d12Resource->Release();
				}

				m_viewHolder = other.m_viewHolder;
				m_d3d12Resource = other.m_d3d12Resource;
				m_descriptorHeap = other.m_descriptorHeap;
				m_desc = other.m_desc;

				if ( m_d3d12Resource )
				{
					m_d3d12Resource->AddRef();
				}
			}

			return *this;
		}

		D3D12ViewBase( D3D12ViewBase&& other )
		{
			*this = std::move( other );
		}

		D3D12ViewBase& operator=( D3D12ViewBase&& other )
		{
			if ( this != &other )
			{
				if ( m_d3d12Resource )
				{
					m_d3d12Resource->Release();
				}

				m_viewHolder = other.m_viewHolder;
				m_d3d12Resource = other.m_d3d12Resource;
				m_descriptorHeap = std::move( other.m_descriptorHeap );
				m_desc = other.m_desc;

				other.m_viewHolder = nullptr;
				other.m_d3d12Resource = nullptr;
				other.m_desc = {};
			}

			return *this;
		}

	protected:
		virtual void FreeResource() override
		{
			m_viewHolder = nullptr;

			if ( m_d3d12Resource )
			{
				m_d3d12Resource->Release();
				m_d3d12Resource = nullptr;
			}

			std::destroy_at( &m_descriptorHeap );
		}

		IResourceViews* m_viewHolder = nullptr;
		ID3D12Resource* m_d3d12Resource = nullptr;
		D3D12DescriptorHeap m_descriptorHeap;
		DescType m_desc = {};
	};

	class D3D12ShaderResourceView : public D3D12ViewBase<ShaderResourceView, D3D12_SHADER_RESOURCE_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<ShaderResourceView, D3D12_SHADER_RESOURCE_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12UnorderedAccessView : public D3D12ViewBase<UnorderedAccessView, D3D12_UNORDERED_ACCESS_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<UnorderedAccessView, D3D12_UNORDERED_ACCESS_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12RenderTargetView : public D3D12ViewBase<RenderTargetView, D3D12_RENDER_TARGET_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<RenderTargetView, D3D12_RENDER_TARGET_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12DepthStencilView : public D3D12ViewBase<DepthStencilView, D3D12_DEPTH_STENCIL_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<DepthStencilView, D3D12_DEPTH_STENCIL_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12ConstantBufferView : public D3D12ViewBase<DeviceDependantResource, D3D12_CONSTANT_BUFFER_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<DeviceDependantResource, D3D12_CONSTANT_BUFFER_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};
}
