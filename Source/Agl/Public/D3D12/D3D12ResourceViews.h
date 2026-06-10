#pragma once

#include "D3D12Api.h"
#include "D3D12BindlessManager.h"
#include "D3D12DescriptorHeapAllocator.h"
#include "ResourceViews.h"

#include <d3d12.h>

namespace agl
{
	template <typename BaseClass, typename DescType>
	class D3D12ViewBase : public BaseClass
	{
	public:
		const D3D12CpuDescriptorHandle& GetCpuHandle() const
		{
			return m_descriptor.m_cpuHandle;
		}

		GraphicsApiResource* GetOwner() const { return m_owner; }

		const DescType& GetD3DDesc() const
		{
			return m_d3dDesc;
		}

		D3D12ViewBase( GraphicsApiResource* owner, ID3D12Resource* d3d12Resource, const DescType& desc ) noexcept :
			m_owner( owner ),
			m_d3d12Resource( d3d12Resource ),
			m_d3dDesc( desc )
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
				m_descriptor = other.m_descriptor;
				m_d3dDesc = other.m_d3dDesc;
			}

			return *this;
		}

		D3D12ViewBase( D3D12ViewBase&& other ) noexcept
		{
			*this = std::move( other );
		}

		D3D12ViewBase& operator=( D3D12ViewBase&& other ) noexcept
		{
			if ( this != &other )
			{
				m_owner = other.m_owner;
				m_d3d12Resource = other.m_d3d12Resource;
				m_descriptor = std::move( other.m_descriptor );
				m_d3dDesc = other.m_d3dDesc;

				other.m_owner = nullptr;
				other.m_d3d12Resource = nullptr;
				other.m_descriptor = {};
				other.m_d3dDesc = {};
			}

			return *this;
		}

	protected:
		virtual void FreeResource() override
		{
			m_owner = nullptr;
			m_d3d12Resource = nullptr;

			D3D12DescriptorPoolForView().Release( m_descriptor );
		}

		GraphicsApiResource* m_owner = nullptr;
		ID3D12Resource* m_d3d12Resource = nullptr;
		D3D12ViewDescriptorHandle m_descriptor;
		DescType m_d3dDesc = {};
	};

	class D3D12ShaderResourceView final : public D3D12ViewBase<ShaderResourceView, D3D12_SHADER_RESOURCE_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<ShaderResourceView, D3D12_SHADER_RESOURCE_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

		virtual int32 GetBindlessHandle() const override;

		void UpdateTextureMips( ID3D12Resource* resource, uint32 mipLevels );

	protected:
		virtual void InitResource() override;
		virtual void FreeResource() override;

	private:
		int32 m_bindlessHandle = NullBindlessHandle;
	};

	class D3D12UnorderedAccessView final : public D3D12ViewBase<UnorderedAccessView, D3D12_UNORDERED_ACCESS_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<UnorderedAccessView, D3D12_UNORDERED_ACCESS_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

		virtual int32 GetBindlessHandle() const override;

	protected:
		virtual void InitResource() override;
		virtual void FreeResource() override;

	private:
		int32 m_bindlessHandle = NullBindlessHandle;
	};

	class D3D12RenderTargetView final : public D3D12ViewBase<RenderTargetView, D3D12_RENDER_TARGET_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<RenderTargetView, D3D12_RENDER_TARGET_VIEW_DESC>;

	public:
		using BaseClass::operator=;

		D3D12RenderTargetView( GraphicsApiResource* owner, ID3D12Resource* d3d12Resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc, const ColorF& clearColor );

		ColorF GetClearColor() const;

	protected:
		virtual void InitResource() override;

	private:
		ColorF m_clearColor = ColorF::Black;
	};

	class D3D12DepthStencilView final : public D3D12ViewBase<DepthStencilView, D3D12_DEPTH_STENCIL_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<DepthStencilView, D3D12_DEPTH_STENCIL_VIEW_DESC>;

	public:
		using BaseClass::operator=;

		D3D12DepthStencilView( GraphicsApiResource* owner, ID3D12Resource* d3d12Resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc, float depthValue, uint8 stencilValue );

		float GetDepthClearValue() const;
		uint8 GetStencilClearValue() const;

	protected:
		virtual void InitResource() override;

	private:
		float m_depthClearValue = 0.f;
		uint8 m_stencilClearValue = 0;
	};

	class D3D12ConstantBufferView final : public D3D12ViewBase<GraphicsApiResource, D3D12_CONSTANT_BUFFER_VIEW_DESC>
	{
		using BaseClass = D3D12ViewBase<GraphicsApiResource, D3D12_CONSTANT_BUFFER_VIEW_DESC>;

	public:
		using BaseClass::BaseClass;
		using BaseClass::operator=;

		virtual int32 GetBindlessHandle() const override;

	protected:
		virtual void InitResource() override;
		virtual void FreeResource() override;

	private:
		int32 m_bindlessHandle = NullBindlessHandle;
	};
}
