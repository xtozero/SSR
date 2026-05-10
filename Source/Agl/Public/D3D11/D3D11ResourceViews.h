#pragma once

#include "ResourceViews.h"

#include <d3d11.h>
#include <wrl/client.h>

namespace agl
{
	template <typename BaseClass, typename ViewType, typename DescType>
	class D3D11ViewBase : public BaseClass
	{
	public:
		using ViewTypePointer = std::add_pointer_t<ViewType>;

		ViewTypePointer Resource() { return m_resource.Get(); }
		ViewTypePointer Resource() const { return m_resource.Get(); }

		const IResourceViews* ViewHolder() const { return m_viewHolder; }

		D3D11ViewBase( IResourceViews* viewHolder, ID3D11Resource* d3d11Resource, const DescType& desc ) :
			m_viewHolder( viewHolder ),
			m_d3d11Resource( d3d11Resource ),
			m_d3dDesc( desc ) {}

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
				m_viewHolder = other.m_viewHolder;
				m_d3d11Resource = other.m_d3d11Resource;
				m_resource = other.m_resource;
				m_d3dDesc = other.m_d3dDesc;
			}

			return *this;
		}

		D3D11ViewBase( D3D11ViewBase&& other ) noexcept
		{
			*this = std::move( other );
		}

		D3D11ViewBase& operator=( D3D11ViewBase&& other ) noexcept
		{
			if ( this != &other )
			{
				m_viewHolder = other.m_viewHolder;
				m_d3d11Resource = other.m_d3d11Resource;
				m_resource = other.m_resource;
				m_d3dDesc = other.m_d3dDesc;

				other.m_viewHolder = nullptr;
				other.m_d3d11Resource = nullptr;
				other.m_resource = nullptr;
				other.m_d3dDesc = {};
			}

			return *this;
		}

	protected:
		virtual void FreeResource() override
		{
			m_viewHolder = nullptr;
			m_d3d11Resource = nullptr;
			m_resource = nullptr;
		}

		IResourceViews* m_viewHolder = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Resource> m_d3d11Resource = nullptr;
		Microsoft::WRL::ComPtr<ViewType> m_resource = nullptr;
		DescType m_d3dDesc = {};
	};

	class D3D11ShaderResourceView final : public D3D11ViewBase<ShaderResourceView, ID3D11ShaderResourceView, D3D11_SHADER_RESOURCE_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<ShaderResourceView, ID3D11ShaderResourceView, D3D11_SHADER_RESOURCE_VIEW_DESC>;

	public:
		void UpdateTextureMips( ID3D11Resource* d3d11Resource, uint32 mipLevels );

		using BaseClass::BaseClass;
		using BaseClass::operator=;

	private:
		virtual void InitResource() override;
	};

	class D3D11UnorderedAccessView final : public D3D11ViewBase<UnorderedAccessView, ID3D11UnorderedAccessView, D3D11_UNORDERED_ACCESS_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<UnorderedAccessView, ID3D11UnorderedAccessView, D3D11_UNORDERED_ACCESS_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

	private:
		virtual void InitResource() override;
	};

	class D3D11RenderTargetView final : public D3D11ViewBase<RenderTargetView, ID3D11RenderTargetView, D3D11_RENDER_TARGET_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<RenderTargetView, ID3D11RenderTargetView, D3D11_RENDER_TARGET_VIEW_DESC>;

	public:
		using BaseClass::operator=;

		D3D11RenderTargetView( IResourceViews* viewHolder, ID3D11Resource* d3d11Resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc, const ColorF& clearColor );

		ColorF GetClearColor() const;

	private:
		virtual void InitResource() override;

		ColorF m_clearColor = ColorF::Black;
	};

	class D3D11DepthStencilView final : public D3D11ViewBase<DepthStencilView, ID3D11DepthStencilView, D3D11_DEPTH_STENCIL_VIEW_DESC>
	{
		using BaseClass = D3D11ViewBase<DepthStencilView, ID3D11DepthStencilView, D3D11_DEPTH_STENCIL_VIEW_DESC>;

	public:
		using BaseClass::operator=;

		D3D11DepthStencilView( IResourceViews* viewHolder, ID3D11Resource* d3d11Resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc, float depthClearValue, uint8 stencilClearValue );

		float GetDepthClearValue() const;
		uint8 GetStencilClearValue() const;

	private:
		virtual void InitResource() override;

		float m_depthClearValue = 0.f;
		uint8 m_stencilClearValue = 0;
	};
}