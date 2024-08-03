#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

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
	};
}