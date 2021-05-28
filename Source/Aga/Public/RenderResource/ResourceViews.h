#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

namespace aga
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
}