#pragma once

#include "GraphicsApiResource.h"
#include "ICommandList.h"
#include "Platform/PlatformTypes.h"
#include "ShaderResource.h"
#include "SizedTypes.h"

#include <filesystem>

class BinaryChunk;

namespace agl
{
	class Buffer;
	class ShaderParameterInfo;
	class ShaderParameterMap;
	class Texture;

	enum class AglType : uint8
	{
		D3D11 = 0,
		D3D12,
		Vulkan
	};

	enum class QueueType : uint8
	{
		Direct,
		Compute,
	};

	class IAgl
	{
	public:
		virtual AglType GetType() const = 0;

		virtual bool BootUp( const engine::PlatformWindowContext& windowCtx ) = 0;
		virtual void OnShutdown() = 0;

		virtual void HandleDeviceLost() = 0;
		virtual void AppSizeChanged() = 0;
		virtual void OnBeginFrameRendering() = 0;
		virtual void OnEndFrameRendering( uint32 oldFrameIndex, uint32 nextFrameIndex ) = 0;
		virtual void WaitGPU() = 0;
		virtual void WaitQueue( QueueType type ) = 0;

		virtual LockedResource Lock( Buffer* buffer, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) = 0;

		virtual LockedResource Lock( Texture* texture, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Texture* texture, uint32 subResource = 0 ) = 0;

		virtual void GetRendererMultiSampleOption( MultiSampleOption* option ) = 0;

		virtual ICommandList* GetCommandList() = 0;
		virtual ICommandList* GetParallelCommandList() = 0;
		virtual IComputeCommandList* GetComputeCommandList() = 0;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const = 0;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const = 0;

		virtual std::filesystem::path GetShaderCacheFilePath() const = 0;

		virtual bool SupportsPSOCache() const = 0;
		virtual bool SupportsPSOLibraryCache() const = 0;
		virtual std::filesystem::path GetPSOCacheFilePath() const = 0;

		virtual bool SupportsHardwareRaytracing() const = 0;

		virtual bool SupportsMeshShader() const = 0;

		virtual bool SupportsWaveIntrinsics() const = 0;

		virtual ~IAgl() = default;

	protected:
		IAgl() = default;
	};
}
