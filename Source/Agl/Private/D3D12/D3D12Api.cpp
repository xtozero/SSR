#include "D3D12Api.h"

#include "Archive.h"

#include "common.h"
#include "Config/DefaultAglConfig.h"

#include "D3D12CommandList.h"

#include "EnumStringMap.h"

#include "IAgl.h"
#include "LibraryTool/InterfaceFactories.h"
#include "Memory/InlineMemoryAllocator.h"

#include "PipelineState.h"

#include "ShaderParameterMap.h"

#include "Texture.h"

#include <array>
#include <cstdlib>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace ::Microsoft::WRL;

namespace
{
	const wchar_t* GetProperProfile( const char* profile )
	{
		if ( std::strncmp( profile, "vs", 2 ) == 0 )
		{
			return L"vs_6_0";
		}
		else if ( std::strncmp( profile, "gs", 2 ) == 0 )
		{
			return L"gs_6_0";
		}
		else if ( std::strncmp( profile, "ps", 2 ) == 0 )
		{
			return L"ps_6_0";
		}
		else if ( std::strncmp( profile, "cs", 2 ) == 0 )
		{
			return L"cs_6_0";
		}

		assert( false && "Invalid shader profile" );
		return L"";
	}
}

namespace agl
{
	class Direct3D12 final : public IAgl
	{
	public:
		virtual AglType GetType() const override;

		virtual bool BootUp() override;
		virtual void OnShutdown() override;

		virtual void HandleDeviceLost() override;
		virtual void AppSizeChanged() override;
		virtual void OnBeginFrameRendering() override;
		virtual void OnEndFrameRendering( uint32 curFrameIndex, uint32 nextFrameIndex ) override;
		virtual void WaitGPU() override;

		virtual LockedResource Lock( Buffer* buffer, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) override;
		virtual LockedResource Lock( Texture* texture, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) override;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) override;
		virtual void UnLock( Texture* texture, uint32 subResource = 0 ) override;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) override;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) override;

		virtual IImmediateCommandList* GetImmediateCommandList() override;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList() const override;
		virtual GraphicsCommandListsBase& GetGraphicsCommandLists() override;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const override;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const override;

		ID3D12Device& GetDevice() const;
		IDXGIFactory7& GetFactory() const;
		ID3D12CommandQueue& GetDirectCommandQueue() const;

		D3D12ResourceUploader& GetUploader();

		uint32 GetFrameIndex() const
		{
			return m_frameIndex;
		}

		virtual ~Direct3D12() override;

	private:
		bool CreateDeviceDependentResource();
		bool CreateDeviceIndependentResource();

#ifdef _DEBUG
		ComPtr<ID3D12Debug> m_debugLayer;
#endif

		ComPtr<IDXGIFactory7> m_factory;

		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_directCommandQueue;

		ComPtr<ID3D12Fence> m_fence;
		std::vector<uint64, InlineAllocator<uint64, 2>> m_fenceValue;
		HANDLE m_fenceEvent;

		ComPtr<IDxcCompiler3> m_compiler;

		uint32 m_frameIndex = 0;

		D3D12CommandList m_commandList;
		std::vector<D3D12GraphicsCommandLists, InlineAllocator<D3D12GraphicsCommandLists, 2>> m_commandLists;

		D3D12ResourceUploader m_uploader;
	};

	AglType Direct3D12::GetType() const
	{
		return AglType::D3D12;
	}

	bool Direct3D12::BootUp()
	{
		if ( CreateDeviceIndependentResource() == false )
		{
			return false;
		}

		if ( CreateDeviceDependentResource() == false )
		{
			return false;
		}

		return true;
	}

	void Direct3D12::OnShutdown()
	{
		m_uploader.WaitUntilCopyCompleted();
	}

	void Direct3D12::HandleDeviceLost()
	{
	}

	void Direct3D12::AppSizeChanged()
	{
	}

	void Direct3D12::OnBeginFrameRendering()
	{
		m_uploader.WaitUntilCopyCompleted();

		m_commandLists[m_frameIndex].Prepare();
	}

	void Direct3D12::OnEndFrameRendering( uint32 curFrameIndex, uint32 nextFrameIndex )
	{
		uint64 fence = m_fenceValue[curFrameIndex];
		[[maybe_unused]] HRESULT hr = m_directCommandQueue->Signal( m_fence.Get(), fence );
		assert( SUCCEEDED( hr ) );

		if ( m_fence->GetCompletedValue() < fence )
		{
			hr = m_fence->SetEventOnCompletion( fence, m_fenceEvent );
			assert( SUCCEEDED( hr ) );
			WaitForSingleObject( m_fenceEvent, INFINITE );
		}

		m_fenceValue[nextFrameIndex] = fence + 1;
		m_frameIndex = nextFrameIndex;
	}

	void Direct3D12::WaitGPU()
	{
		uint64 fence = m_fenceValue[m_frameIndex];
		[[maybe_unused]] HRESULT hr = m_directCommandQueue->Signal( m_fence.Get(), fence );
		assert( SUCCEEDED( hr ) );

		if ( m_fence->GetCompletedValue() < fence )
		{
			hr = m_fence->SetEventOnCompletion( fence, m_fenceEvent );
			assert( SUCCEEDED( hr ) );
			WaitForSingleObject( m_fenceEvent, INFINITE );
		}

		++m_fenceValue[m_frameIndex];
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

	GraphicsCommandListsBase& Direct3D12::GetGraphicsCommandLists()
	{
		return m_commandLists[m_frameIndex];
	}

	BinaryChunk Direct3D12::CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const
	{
		DxcBuffer buffer = {
			.Ptr = source.Data(),
			.Size = source.Size(),
			.Encoding = DXC_CP_ACP
		};

		std::vector<const wchar_t*> args;

		// entry point
		args.push_back( L"-E" );
		args.push_back( L"main" );

		// target profile
		args.push_back( L"-T" );
		args.push_back( GetProperProfile( profile ) );

#if _DEBUG
		args.push_back( L"-Zs" );
#endif

		// defines
		args.push_back( L"-D" );
		args.push_back( L"D3D12=1");

		constexpr int MaxDefineLen = 256;
		std::vector<std::array<wchar_t, MaxDefineLen>> defineStorage;
		for ( uint32 i = 0; i < defines.size(); i += 2 )
		{
			if ( defines[i] == nullptr || defines[i + 1] == nullptr )
			{
				continue;
			}

			std::array<char, MaxDefineLen> define;

#if _WIN32
			sprintf_s( define.data(), MaxDefineLen, "%s=%s", defines[i], defines[i + 1] );
#else
			std::sprintf( define.data(), "%s=%s", defines[i], defines[i + 1] );
#endif

			defineStorage.emplace_back();
			std::array<wchar_t, MaxDefineLen>& wDefine = defineStorage.back();

#if _WIN32
			size_t numConverted = 0;
			mbstowcs_s( &numConverted, wDefine.data(), MaxDefineLen, define.data(), MaxDefineLen );
#else
			std::mbstowcs( wDefine.data(), define.data(), MaxDefineLen );
#endif

			args.push_back( L"-D" );
			args.push_back( wDefine.data() );
		}

		ComPtr<IDxcResult> results;
		m_compiler->Compile( &buffer
			, args.data()
			, static_cast<uint32>( args.size() )
			, nullptr
			, IID_PPV_ARGS( results.GetAddressOf() ) );

		HRESULT hResult = S_OK;
		results->GetStatus( &hResult );

		assert( SUCCEEDED( hResult ) );

		ComPtr<IDxcBlob> compiledBinary = nullptr;
		ComPtr<IDxcBlobUtf16> shaderName = nullptr;
		results->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( compiledBinary.GetAddressOf() ), shaderName.GetAddressOf() );
		
		assert( compiledBinary.Get() != nullptr );

		BinaryChunk binary( compiledBinary->GetBufferSize() );
		std::memcpy( binary.Data(), compiledBinary->GetBufferPointer(), compiledBinary->GetBufferSize() );

		return binary;
	}

	bool Direct3D12::BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const
	{
		return false;
	}

	ID3D12Device& Direct3D12::GetDevice() const
	{
		return *m_device.Get();
	}

	IDXGIFactory7& Direct3D12::GetFactory() const
	{
		return *m_factory.Get();
	}

	ID3D12CommandQueue& Direct3D12::GetDirectCommandQueue() const
	{
		return *m_directCommandQueue.Get();
	}

	D3D12ResourceUploader& Direct3D12::GetUploader()
	{
		return m_uploader;
	}

	Direct3D12::~Direct3D12()
	{
		CloseHandle( m_fenceEvent );
	}

	bool Direct3D12::CreateDeviceDependentResource()
	{
		D3D_FEATURE_LEVEL d3dFeatureLevel[] = {
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		HRESULT hr = E_FAIL;

#ifdef _DEBUG
		hr = D3D12GetDebugInterface( IID_PPV_ARGS( &m_debugLayer ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		m_debugLayer->EnableDebugLayer();
#endif

		for ( uint32 i = 0; i < _countof( d3dFeatureLevel ); ++i )
		{
			hr = D3D12CreateDevice( nullptr
				, d3dFeatureLevel[i]
				, IID_PPV_ARGS( m_device.GetAddressOf() ) );

			if ( SUCCEEDED( hr ) )
			{
				break;
			}
		}

		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D12_COMMAND_QUEUE_DESC desc = {
				.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
				.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
				.NodeMask = 0
		};

		hr = m_device->CreateCommandQueue( &desc, IID_PPV_ARGS( m_directCommandQueue.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_device->CreateFence( m_fenceValue[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_fence ) );
		++m_fenceValue[m_frameIndex];

		if ( m_uploader.Initialize() == false )
		{
			return false;
		}

		return true;
	}

	bool Direct3D12::CreateDeviceIndependentResource()
	{
		ComPtr<IDXGIFactory2> factory;

		uint32 factoryFlag = 0;
#ifdef _DEBUG
		factoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif
		HRESULT hr = CreateDXGIFactory2( factoryFlag, IID_PPV_ARGS( factory.GetAddressOf() ) );

		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = factory.As( &m_factory );
		if ( FAILED( hr ) )
		{
			return false;
		}

		m_fenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
		if ( m_fenceEvent == nullptr )
		{
			return false;
		}

		m_fenceValue.resize( DefaultAgl::GetBufferCount(), 0 );
		m_commandLists.resize( DefaultAgl::GetBufferCount() );

		hr = DxcCreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( m_compiler.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	ID3D12CommandQueue& D3D12DirectCommandQueue()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetDirectCommandQueue();
	}

	ID3D12Device& D3D12Device()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetDevice();
	}

	IDXGIFactory7& D3D12Factory()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetFactory();
	}

	D3D12ResourceUploader& D3D12Uploader()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetUploader();
	}

	Owner<IAgl*> CreateD3D12GraphicsApi()
	{
		return new Direct3D12();
	}

	uint32 GetFrameIndex()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetFrameIndex();
	}
}
