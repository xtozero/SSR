#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ICommandList.h"
#include "SizedTypes.h"

#include <memory>

namespace aga
{
	class Buffer;

	class IAga
	{
	public:
		virtual bool BootUp() = 0;
		virtual void HandleDeviceLost() = 0;
		virtual void AppSizeChanged() = 0;
		virtual void WaitGPU() = 0;

		virtual void* Lock( Buffer* buffer, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) = 0;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) = 0;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) = 0;

		virtual IImmediateCommandList* GetImmediateCommandList() = 0;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList() const = 0;

		virtual ~IAga() = default;

	protected:
		IAga() = default;
	};
}
