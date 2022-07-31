#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ICommandList.h"
#include "SizedTypes.h"

#include <memory>

class BinaryChunk;

namespace aga
{
	class Buffer;
	class ShaderParameterInfo;
	class ShaderParameterMap;
	class Texture;

	class IAga
	{
	public:
		virtual bool BootUp() = 0;
		virtual void HandleDeviceLost() = 0;
		virtual void AppSizeChanged() = 0;
		virtual void WaitGPU() = 0;

		virtual LockedResource Lock( Buffer* buffer, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 ) = 0;
		virtual LockedResource Lock( Texture* texture, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Texture* texture, uint32 subResource = 0 ) = 0;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) = 0;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) = 0;

		virtual IImmediateCommandList* GetImmediateCommandList() = 0;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList() const = 0;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const = 0;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const = 0;

		virtual ~IAga() = default;

	protected:
		IAga() = default;
	};
}
