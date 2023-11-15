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

		GraphicsApiResource* GetOwner() const { return m_owner; }

		const DescType& GetDesc() const
		{
			return m_desc;
		}

		D3D12ViewBase( GraphicsApiResource* owner, ID3D12Resource* d3d11Resource, const DescType& desc ) noexcept :
			m_owner( owner ),
			m_d3d12Resource( d3d11Resource ),
			m_desc( desc )
		{}

		virtual ~D3D12ViewBase() override
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
				m_owner = other.m_owner;
				m_d3d12Resource = other.m_d3d12Resource;
				m_descriptorHeap = other.m_descriptorHeap;
				m_desc = other.m_desc;
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
				m_owner = other.m_owner;
				m_d3d12Resource = other.m_d3d12Resource;
				m_descriptorHeap = std::move( other.m_descriptorHeap );
				m_desc = other.m_desc;

				other.m_owner = nullptr;
				other.m_d3d12Resource = nullptr;
				other.m_desc = {};
			}

			return *this;
		}

	protected:
		virtual void FreeResource() override
		{
			m_owner = nullptr;
			m_d3d12Resource = nullptr;

			std::destroy_at( &m_descriptorHeap );
		}

		GraphicsApiResource* m_owner = nullptr;
		ID3D12Resource* m_d3d12Resource = nullptr;
		D3D12DescriptorHeap m_descriptorHeap;
		DescType m_desc = {};
	};

	class D3D12ShaderResourceView final : public D3D12ViewBase<ShaderResourceView, D3D12_SHADER_RESOURCE_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<ShaderResourceView, D3D12_SHADER_RESOURCE_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12UnorderedAccessView final : public D3D12ViewBase<UnorderedAccessView, D3D12_UNORDERED_ACCESS_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<UnorderedAccessView, D3D12_UNORDERED_ACCESS_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12RenderTargetView final : public D3D12ViewBase<RenderTargetView, D3D12_RENDER_TARGET_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<RenderTargetView, D3D12_RENDER_TARGET_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12DepthStencilView final : public D3D12ViewBase<DepthStencilView, D3D12_DEPTH_STENCIL_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<DepthStencilView, D3D12_DEPTH_STENCIL_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D12ConstantBufferView final : public D3D12ViewBase<DeviceDependantResource, D3D12_CONSTANT_BUFFER_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<DeviceDependantResource, D3D12_CONSTANT_BUFFER_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};
}
