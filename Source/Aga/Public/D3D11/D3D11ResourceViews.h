#pragma once

#include "ResourceViews.h"

#include <d3d11.h>

namespace aga
{
	template <typename BaseClass, typename ViewType, typename DescType>
	class D3D11ViewBase : public BaseClass
	{
	public:
		ViewType Resource( ) { return m_view; }
		const ViewType Resource( ) const { return m_view; }

		D3D11ViewBase( ) = default;
		D3D11ViewBase( ID3D11Resource* resource, const DescType& desc ) : m_resourceRef( resource ), m_desc( desc )
		{
			if ( m_resourceRef )
			{
				m_resourceRef->AddRef( );
			}
		}

		~D3D11ViewBase( )
		{
			this->Free( );
		}

		D3D11ViewBase( const D3D11ViewBase& other )
		{
			*this = other;
		}

		D3D11ViewBase& operator=( const D3D11ViewBase& other )
		{
			if ( this != &other )
			{
				if ( m_view )
				{
					m_view->Release( );
				}

				if ( m_resourceRef )
				{
					m_resourceRef->Release( );
				}

				m_resourceRef = other.m_resourceRef;
				m_view = other.m_view;
				m_desc = other.m_desc;

				if ( m_resourceRef )
				{
					m_resourceRef->AddRef( );
				}

				if ( m_view )
				{
					this->m_srv->AddRef( );
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
				if ( m_view )
				{
					m_view->Release( );
				}

				if ( m_resourceRef )
				{
					m_resourceRef->Release( );
				}

				m_resourceRef = other.m_resourceRef;
				m_view = other.m_view;
				m_desc = other.m_desc;

				other.m_resourceRef = nullptr;
				other.m_view = nullptr;
				other.m_desc = {};
			}

			return *this;
		}

	protected:
		virtual void FreeResource( ) override
		{
			if ( m_resourceRef )
			{
				m_resourceRef->Release( );
				m_resourceRef = nullptr;
			}

			if ( m_view )
			{
				m_view->Release( );
				m_view = nullptr;
			}
		}

		ID3D11Resource* m_resourceRef = nullptr;
		ViewType m_view = nullptr;
		DescType m_desc = {};
	};

	class D3D11ShaderResourceView : public D3D11ViewBase<ShaderResourceView, ID3D11ShaderResourceView*, D3D11_SHADER_RESOURCE_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<ShaderResourceView, ID3D11ShaderResourceView*, D3D11_SHADER_RESOURCE_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource( ) override;
	};

	class D3D11UnorderedAccessView : public D3D11ViewBase<UnorderedAccessView, ID3D11UnorderedAccessView*, D3D11_UNORDERED_ACCESS_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<UnorderedAccessView, ID3D11UnorderedAccessView*, D3D11_UNORDERED_ACCESS_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource( ) override;
	};

	class D3D11RenderTargetView : public D3D11ViewBase<RenderTargetView, ID3D11RenderTargetView*, D3D11_RENDER_TARGET_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<RenderTargetView, ID3D11RenderTargetView*, D3D11_RENDER_TARGET_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource( ) override;
	};

	class D3D11DepthStencilView : public D3D11ViewBase<DepthStencilView, ID3D11DepthStencilView*, D3D11_DEPTH_STENCIL_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<DepthStencilView, ID3D11DepthStencilView*, D3D11_DEPTH_STENCIL_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	protected:
		virtual void InitResource( ) override;
	};
}