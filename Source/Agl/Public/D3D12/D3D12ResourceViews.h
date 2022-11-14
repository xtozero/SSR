#pragma once

#include "ResourceViews.h"

#include <d3d12.h>

namespace agl
{
	template <typename BaseClass, typename ViewType, typename DescType>
	class D3D12ViewBase : public BaseClass
	{
	public:
		ViewType Resource() { return m_resource; }
		const ViewType Resource() const { return m_resource; }

		const IResourceViews* ViewHolder() const { return m_viewHolder; }

		D3D12ViewBase( IResourceViews* viewHolder, ID3D12Resource* d3d11Resource, const DescType& desc ) :
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
				if ( m_resource )
				{
					m_resource->Release();
				}

				if ( m_d3d12Resource )
				{
					m_d3d12Resource->Release();
				}

				m_viewHolder = other.m_viewHolder;
				m_d3d12Resource = other.m_d3d12Resource;
				m_resource = other.m_resource;
				m_desc = other.m_desc;

				if ( m_d3d12Resource )
				{
					m_d3d12Resource->AddRef();
				}

				if ( m_resource )
				{
					m_resource->AddRef();
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
				if ( m_resource )
				{
					m_resource->Release();
				}

				if ( m_d3d12Resource )
				{
					m_d3d12Resource->Release();
				}

				m_viewHolder = other.m_viewHolder;
				m_d3d12Resource = other.m_d3d12Resource;
				m_resource = other.m_resource;
				m_desc = other.m_desc;

				other.m_viewHolder = nullptr;
				other.m_d3d12Resource = nullptr;
				other.m_resource = nullptr;
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

			if ( m_resource )
			{
				m_resource->Release();
				m_resource = nullptr;
			}
		}

		IResourceViews* m_viewHolder = nullptr;
		ID3D12Resource* m_d3d12Resource = nullptr;
		ViewType m_resource = nullptr;
		DescType m_desc = {};
	};

	class D3D12RenderTargetView : public D3D12ViewBase<RenderTargetView, ID3D12DescriptorHeap*, D3D12_RENDER_TARGET_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<RenderTargetView, ID3D12DescriptorHeap*, D3D12_RENDER_TARGET_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE	m_cpuHandle;
	};
}
