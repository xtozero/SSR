#pragma once

#include "ResourceViews.h"

#include <d3d11.h>

namespace agl
{
	template <typename BaseClass, typename ViewType, typename DescType>
	class D3D11ViewBase : public BaseClass
	{
	public:
		ViewType Resource() { return m_resource; }
		const ViewType Resource() const { return m_resource; }

		const IResourceViews* ViewHolder() const { return m_viewHolder; }

		D3D11ViewBase( IResourceViews* viewHolder, ID3D11Resource* d3d11Resource, const DescType& desc ) :
			m_viewHolder( viewHolder ),
			m_d3d11Resource( d3d11Resource ),
			m_desc( desc )
		{
			if ( m_d3d11Resource )
			{
				m_d3d11Resource->AddRef();
			}
		}

		virtual ~D3D11ViewBase() override
		{
			this->Free();
		}

		D3D11ViewBase( const D3D11ViewBase& other )
		{
			*this = other;
		}

		D3D11ViewBase& operator=( const D3D11ViewBase& other )
		{
			if ( this != &other )
			{
				if ( m_resource )
				{
					m_resource->Release();
				}

				if ( m_d3d11Resource )
				{
					m_d3d11Resource->Release();
				}

				m_viewHolder = other.m_viewHolder;
				m_d3d11Resource = other.m_d3d11Resource;
				m_resource = other.m_resource;
				m_desc = other.m_desc;

				if ( m_d3d11Resource )
				{
					m_d3d11Resource->AddRef();
				}

				if ( m_resource )
				{
					m_resource->AddRef();
				}
			}

			return *this;
		}

		D3D11ViewBase( D3D11ViewBase&& other )
		{
			*this = std::move( other );
		}

		D3D11ViewBase& operator=( D3D11ViewBase&& other )
		{
			if ( this != &other )
			{
				if ( m_resource )
				{
					m_resource->Release();
				}

				if ( m_d3d11Resource )
				{
					m_d3d11Resource->Release();
				}

				m_viewHolder = other.m_viewHolder;
				m_d3d11Resource = other.m_d3d11Resource;
				m_resource = other.m_resource;
				m_desc = other.m_desc;

				other.m_viewHolder = nullptr;
				other.m_d3d11Resource = nullptr;
				other.m_resource = nullptr;
				other.m_desc = {};
			}

			return *this;
		}

	protected:
		virtual void FreeResource() override
		{
			m_viewHolder = nullptr;

			if ( m_d3d11Resource )
			{
				m_d3d11Resource->Release();
				m_d3d11Resource = nullptr;
			}

			if ( m_resource )
			{
				m_resource->Release();
				m_resource = nullptr;
			}
		}

		IResourceViews* m_viewHolder = nullptr;
		ID3D11Resource* m_d3d11Resource = nullptr;
		ViewType m_resource = nullptr;
		DescType m_desc = {};
	};

	class D3D11ShaderResourceView : public D3D11ViewBase<ShaderResourceView, ID3D11ShaderResourceView*, D3D11_SHADER_RESOURCE_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<ShaderResourceView, ID3D11ShaderResourceView*, D3D11_SHADER_RESOURCE_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D11UnorderedAccessView : public D3D11ViewBase<UnorderedAccessView, ID3D11UnorderedAccessView*, D3D11_UNORDERED_ACCESS_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<UnorderedAccessView, ID3D11UnorderedAccessView*, D3D11_UNORDERED_ACCESS_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D11RenderTargetView : public D3D11ViewBase<RenderTargetView, ID3D11RenderTargetView*, D3D11_RENDER_TARGET_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<RenderTargetView, ID3D11RenderTargetView*, D3D11_RENDER_TARGET_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};

	class D3D11DepthStencilView : public D3D11ViewBase<DepthStencilView, ID3D11DepthStencilView*, D3D11_DEPTH_STENCIL_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<DepthStencilView, ID3D11DepthStencilView*, D3D11_DEPTH_STENCIL_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource() override;
	};
}