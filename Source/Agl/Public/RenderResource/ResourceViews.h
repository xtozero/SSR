#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

namespace agl
{
	class ShaderResourceView : public DeviceDependantResource
	{
	};

	class UnorderedAccessView : public DeviceDependantResource
	{
	};

	class RenderTargetView : public DeviceDependantResource
	{
	};

	class DepthStencilView : public DeviceDependantResource
	{
	};

	class IResourceViews
	{
	public:
		virtual ShaderResourceView* SRV() = 0;
		virtual const ShaderResourceView* SRV() const = 0;

		virtual UnorderedAccessView* UAV() = 0;
		virtual const UnorderedAccessView* UAV() const = 0;

		virtual RenderTargetView* RTV() = 0;
		virtual const RenderTargetView* RTV() const = 0;

		virtual DepthStencilView* DSV() = 0;
		virtual const DepthStencilView* DSV() const = 0;
	};
}