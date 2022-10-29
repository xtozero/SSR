#include "D3D12Api.h"

#include "Archive.h"

#include "common.h"

#include "D3D12CommandList.h"

#include "EnumStringMap.h"

#include "IAgl.h"
#include "LibraryTool/InterfaceFactories.h"

#include "PipelineState.h"

#include "ShaderParameterMap.h"

#include "Texture.h"

#include <d3d12.h>
#include <wrl/client.h>

using namespace ::Microsoft::WRL;

namespace agl
{
	class Direct3D12 : public IAgl
	{
	public:
		virtual bool BootUp() override;
		virtual void HandleDeviceLost() override;
		virtual void AppSizeChanged() override;
		virtual void WaitGPU() override;

		virtual LockedResource Lock( Buffer* buffer, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) override;
		virtual LockedResource Lock( Texture* texture, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) override;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) override;
		virtual void UnLock( Texture* texture, uint32 subResource = 0 ) override;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) override;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) override;

		virtual IImmediateCommandList* GetImmediateCommandList() override;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList() const override;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const override;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const override;

		ID3D12Device& GetDevice() const
		{
			return *m_pD3D12Device.Get();
		}

	private:
		ComPtr<ID3D12Device> m_pD3D12Device;

		D3D12CommandList m_commandList;
	};

	bool Direct3D12::BootUp()
	{
		return true;
	}

	void Direct3D12::HandleDeviceLost()
	{
	}

	void Direct3D12::AppSizeChanged()
	{
	}

	void Direct3D12::WaitGPU()
	{
	}

	LockedResource Direct3D12::Lock( Buffer* buffer, ResourceLockFlag lockFlag, uint32 subResource )
	{
		return LockedResource();
	}

	LockedResource Direct3D12::Lock( Texture* texture, ResourceLockFlag lockFlag, uint32 subResource )
	{
		return LockedResource();
	}

	void Direct3D12::UnLock( Buffer* buffer, uint32 subResource )
	{
	}

	void Direct3D12::UnLock( Texture* texture, uint32 subResource )
	{
	}

	void Direct3D12::Copy( Buffer* dst, Buffer* src, uint32 size )
	{
	}

	void Direct3D12::GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option )
	{
	}

	IImmediateCommandList* Direct3D12::GetImmediateCommandList()
	{
		return &m_commandList;
	}

	std::unique_ptr<IDeferredCommandList> Direct3D12::CreateDeferredCommandList() const
	{
		return std::make_unique<D3D12DeferredCommandList>();
	}

	BinaryChunk Direct3D12::CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const
	{
		return BinaryChunk();
	}

	bool Direct3D12::BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const
	{
		return false;
	}

	ID3D12Device& D3D12Device()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetDevice();
	}

	Owner<IAgl*> CreateD3D12GraphicsApi()
	{
		return new Direct3D12();
	}
}
