#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ICommandList.h"
#include "SizedTypes.h"

#include <memory>

class BinaryChunk;

namespace agl
{
	class Buffer;
	class GraphicsCommandListsBase;
	class ShaderParameterInfo;
	class ShaderParameterMap;
	class Texture;

	class IAgl
	{
	public:
		virtual AglType GetType() const = 0;

		virtual bool BootUp() = 0;
		virtual void OnShutdown() = 0;

		virtual void HandleDeviceLost() = 0;
		virtual void AppSizeChanged() = 0;
		virtual void OnBeginFrameRendering() = 0;
		virtual void OnEndFrameRendering( uint32 oldFrameIndex, uint32 newFrameIndex ) = 0;
		virtual void WaitGPU() = 0;

		virtual LockedResource Lock( Buffer* buffer, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) = 0;
		virtual LockedResource Lock( Texture* texture, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Texture* texture, uint32 subResource = 0 ) = 0;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) = 0;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) = 0;

		virtual IImmediateCommandList* GetImmediateCommandList() = 0;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList() const = 0;
		virtual GraphicsCommandListsBase& GetGraphicsCommandLists() = 0;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const = 0;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const = 0;

		virtual ~IAgl() = default;

	protected:
		IAgl() = default;
	};
}
