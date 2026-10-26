#pragma once

#include "GraphicsApiResource.h"

#include <span>

namespace agl
{
	class ShaderResourceView : public GraphicsApiResource
	{
	};

	class UnorderedAccessView : public GraphicsApiResource
	{
	};

	class RenderTargetView : public GraphicsApiResource
	{
	};

	class DepthStencilView : public GraphicsApiResource
	{
	};

	template<typename ViewType>
	struct RasterOutputAttachment
	{
		ViewType* m_view = nullptr;
		RasterOutputLoadAction m_loadAction = RasterOutputLoadAction::NoAction;
	};

	using RenderTargetAttachment = RasterOutputAttachment<RenderTargetView>;
	using DepthStencilAttachment = RasterOutputAttachment<DepthStencilView>;

	struct RasterOutputAttachments
	{
		std::span<RenderTargetAttachment> m_rtAttachments;
		DepthStencilAttachment m_dsAttachment;
	};

	class IResourceViews
	{
	public:
		virtual ShaderResourceView* SRV() = 0;
		virtual const ShaderResourceView* SRV() const = 0;

		virtual UnorderedAccessView* UAV( uint32 mipSlice = 0 ) = 0;
		virtual const UnorderedAccessView* UAV( uint32 mipSlice = 0 ) const = 0;

		virtual RenderTargetView* RTV() = 0;
		virtual const RenderTargetView* RTV() const = 0;

		virtual DepthStencilView* DSV() = 0;
		virtual const DepthStencilView* DSV() const = 0;

		virtual ~IResourceViews() = default;
	};
}