#include "D3D12Api.h"

#include "Archive.h"

#include "common.h"
#include "Config/DefaultAglConfig.h"

#include "D3D12CommandList.h"
#include "D3D12NullDescriptor.h"

#include "EnumStringMap.h"

#include "IAgl.h"
#include "LibraryTool/InterfaceFactories.h"
#include "Memory/InlineMemoryAllocator.h"

#include "PipelineState.h"

#include "ShaderParameterMap.h"

#include "Texture.h"

#include "d3d12shader.h"
#include "dxcapi.h"

#include <array>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace ::Microsoft::WRL;

namespace
{
	using ::agl::ShaderType;

	ShaderType ConvertShaderVersionToType( uint32 shaderVersion )
	{
		D3D12_SHADER_VERSION_TYPE shaderType = static_cast<D3D12_SHADER_VERSION_TYPE>( D3D12_SHVER_GET_TYPE( shaderVersion ) );

		switch ( shaderType )
		{
		case D3D12_SHVER_PIXEL_SHADER:
			return ShaderType::PS;
			break;
		case D3D12_SHVER_VERTEX_SHADER:
			return ShaderType::VS;
			break;
		case D3D12_SHVER_GEOMETRY_SHADER:
			return ShaderType::GS;
			break;
		case D3D12_SHVER_HULL_SHADER:
			return ShaderType::HS;
			break;
		case D3D12_SHVER_DOMAIN_SHADER:
			return ShaderType::DS;
			break;
		case D3D12_SHVER_COMPUTE_SHADER:
			return ShaderType::CS;
			break;
		default:
			break;
		}

		return ShaderType::None;
	}

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
	void ExtractShaderParameters( ID3D12ShaderReflection* pReflector, ShaderParameterMap& parameterMap )
	{
		D3D12_SHADER_DESC shaderDesc = {};
		HRESULT hResult = pReflector->GetDesc( &shaderDesc );
		assert( SUCCEEDED( hResult ) );

		ShaderType shaderType = ConvertShaderVersionToType( shaderDesc.Version );

		// Get the resources
		for ( uint32 i = 0; i < shaderDesc.BoundResources; ++i )
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
			hResult = pReflector->GetResourceBindingDesc( i, &bindDesc );
			assert( SUCCEEDED( hResult ) );

			ShaderParameterType parameterType = ShaderParameterType::ConstantBuffer;
			uint32 parameterSize = 0;

			if ( bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER )
			{
				parameterType = ShaderParameterType::ConstantBuffer;

				ID3D12ShaderReflectionConstantBuffer* constBufferReflection = pReflector->GetConstantBufferByName( bindDesc.Name );
				if ( constBufferReflection )
				{
					D3D12_SHADER_BUFFER_DESC shaderBuffDesc;
					constBufferReflection->GetDesc( &shaderBuffDesc );

					parameterSize = shaderBuffDesc.Size;

					for ( uint32 j = 0; j < shaderBuffDesc.Variables; ++j )
					{
						ID3D12ShaderReflectionVariable* variableReflection = constBufferReflection->GetVariableByIndex( j );
						D3D12_SHADER_VARIABLE_DESC shaderVarDesc;
						variableReflection->GetDesc( &shaderVarDesc );

						parameterMap.AddParameter( shaderVarDesc.Name, shaderType, ShaderParameterType::ConstantBufferValue, bindDesc.BindPoint, shaderVarDesc.StartOffset, shaderVarDesc.Size );
					}
				}
			}
			else if ( bindDesc.Type == D3D_SIT_TEXTURE )
			{
				parameterType = ShaderParameterType::SRV;
			}
			else if ( bindDesc.Type == D3D_SIT_SAMPLER )
			{
				parameterType = ShaderParameterType::Sampler;
			}
			else if ( bindDesc.Type == D3D_SIT_UAV_RWTYPED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||
				bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS || bindDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED ||
				bindDesc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER )
			{
				parameterType = ShaderParameterType::UAV;
			}
			else if ( bindDesc.Type == D3D_SIT_STRUCTURED || bindDesc.Type == D3D_SIT_BYTEADDRESS )
			{
				parameterType = ShaderParameterType::SRV;
			}
			else
			{
				assert( false && "Unexpected case" );
			}

			parameterMap.AddParameter( bindDesc.Name, shaderType, parameterType, bindDesc.BindPoint, 0, parameterSize );
		}
	}

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

		virtual void GetRendererMultiSampleOption( MultiSampleOption* option ) override;

		virtual ICommandList* GetCommandList() override;
		virtual IParallelCommandList* GetParallelCommandList() override;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const override;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const override;

		ID3D12Device& GetDevice() const;
		IDXGIFactory7& GetFactory() const;
		ID3D12CommandQueue& GetDirectCommandQueue() const;

		D3D12ResourceUploader& GetUploader();

		D3D12ResourceAllocator& GetAllocator();

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
		ComPtr<IDxcLibrary> m_dxcLibrary;
		ComPtr<IDxcContainerReflection> m_reflection;

		uint32 m_frameIndex = 0;

		std::vector<D3D12CommandList, InlineAllocator<D3D12CommandList, 2>> m_commandList;

		D3D12ResourceUploader m_uploader;

		D3D12ResourceAllocator m_allocator;
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

		// Dereferencing rendering resources
		for ( auto& cmdList : m_commandList )
		{
			cmdList.Prepare();
		}

		D3D12NullDescriptorStorage::Clear();
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

		m_commandList[m_frameIndex].Prepare();
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
		auto d3d12Buffer = static_cast<D3D12Buffer*>( buffer );
		if ( d3d12Buffer == nullptr )
		{
			return {};
		}

		return d3d12Buffer->Lock( subResource );
	}

	LockedResource Direct3D12::Lock( Texture* texture, ResourceLockFlag lockFlag, uint32 subResource )
	{
		return LockedResource();
	}

	void Direct3D12::UnLock( Buffer* buffer, uint32 subResource )
	{
		auto d3d12Buffer = static_cast<D3D12Buffer*>( buffer );
		if ( d3d12Buffer == nullptr )
		{
			return;
		}

		d3d12Buffer->UnLock( subResource );
	}

	void Direct3D12::UnLock( Texture* texture, uint32 subResource )
	{
	}

	void Direct3D12::Copy( Buffer* dst, Buffer* src, uint32 size )
	{
	}

	void Direct3D12::GetRendererMultiSampleOption( MultiSampleOption* option )
	{
	}

	ICommandList* Direct3D12::GetCommandList()
	{
		return &m_commandList[m_frameIndex];
	}

	IParallelCommandList* Direct3D12::GetParallelCommandList()
	{
		return &m_commandList[m_frameIndex].GetParallelCommandList();
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

		constexpr int32 MaxDefineLen = 256;
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

		BinaryChunk binary( static_cast<uint32>( compiledBinary->GetBufferSize() ) );
		std::memcpy( binary.Data(), compiledBinary->GetBufferPointer(), compiledBinary->GetBufferSize() );

		return binary;
	}

	bool Direct3D12::BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const
	{
		ComPtr<IDxcBlobEncoding> binaryBlob;
		HRESULT hr = m_dxcLibrary->CreateBlobWithEncodingOnHeapCopy( byteCode.Data(), byteCode.Size(), CP_ACP, binaryBlob.GetAddressOf() );
		if ( FAILED( hr ) )
		{
			assert(false && "CreateBlobWithEncodingOnHeapCopy was Failed");
			return false;
		}

		hr = m_reflection->Load( binaryBlob.Get() );
		if ( FAILED( hr ) )
		{
			assert( false && "Binary Loading was Failed" );
			return false;
		}

		uint32 shaderIndex = 0;
		hr = m_reflection->FindFirstPartKind( DXC_FOURCC( 'D', 'X', 'I', 'L' ), &shaderIndex );
		if ( FAILED( hr ) )
		{
			assert( false && "DXIL Searchint was Failed" );
			return false;
		}

		ComPtr<ID3D12ShaderReflection> shaderReflection;
		hr = m_reflection->GetPartReflection( shaderIndex, IID_PPV_ARGS( shaderReflection.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			assert( false && "GetPartReflection was Failed" );
			return false;
		}

		ExtractShaderParameters( shaderReflection.Get(), outParameterMap );
		BuildShaderParameterInfo( outParameterMap.GetParameterMap(), outParameterInfo );

		return true;
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

	D3D12ResourceAllocator& Direct3D12::GetAllocator()
	{
		return m_allocator;
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

		m_commandList.resize( DefaultAgl::GetBufferCount() );
		for ( D3D12CommandList& frameCommandList : m_commandList )
		{
			frameCommandList.Initialize();
		}

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

		hr = DxcCreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( m_compiler.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = DxcCreateInstance( CLSID_DxcLibrary, IID_PPV_ARGS( m_dxcLibrary.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = DxcCreateInstance( CLSID_DxcContainerReflection, IID_PPV_ARGS( m_reflection.GetAddressOf() ) );
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

	D3D12ResourceAllocator& D3D12Allocator()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetAllocator();
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
