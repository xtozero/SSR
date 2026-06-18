#include "D3D12Api.h"

#include "Archive.h"

#include "Config/DefaultAglConfig.h"
#include "Core/Paths.h"

#include "D3D12BindlessManager.h"
#include "D3D12CommandList.h"
#include "D3D12DescriptorHeapAllocator.h"
#include "D3D12FrameResourceCollection.h"
#include "D3D12NullDescriptor.h"
#include "D3D12Query.h"
#include "D3D12ResourceManager.h"
#include "D3D12ResourceUploader.h"

#include "IAgl.h"
#include "LibraryTool/InterfaceFactories.h"
#include "Memory/InlineMemoryAllocator.h"

#include "ShaderParameterMap.h"

#include "d3d12shader.h"
#include "dxcapi.h"

#include <array>
#include <dxgi1_6.h>
#include <wrl/client.h>

using ::Microsoft::WRL::ComPtr;

namespace
{
	bool IsAdapterIntegrated( IDXGIAdapter1* adapter )
	{
		ComPtr<IDXGIAdapter3> adapter3;
		adapter->QueryInterface( IID_PPV_ARGS( adapter3.GetAddressOf() ) );

		DXGI_QUERY_VIDEO_MEMORY_INFO nonLocalVideoMemoryInfo = {};
		if ( adapter3.Get() && SUCCEEDED( adapter3->QueryVideoMemoryInfo( 0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocalVideoMemoryInfo ) ) )
		{
			return nonLocalVideoMemoryInfo.Budget == 0;
		}

		return true;
	}
}

namespace agl
{
	struct D3D12ShaderReflectionLibrary
	{
		static ShaderType ResolveBindingStageFromVersion( uint32 shaderVersion )
		{
			auto version = static_cast<D3D12_SHADER_VERSION_TYPE>( D3D12_SHVER_GET_TYPE( shaderVersion ) );

			switch ( version )
			{
			case D3D12_SHVER_PIXEL_SHADER:
				return ShaderType::Pixel;
			case D3D12_SHVER_VERTEX_SHADER:
				return ShaderType::Vertex;
			case D3D12_SHVER_GEOMETRY_SHADER:
				return ShaderType::Geometry;
			case D3D12_SHVER_HULL_SHADER:
				return ShaderType::Hull;
			case D3D12_SHVER_DOMAIN_SHADER:
				return ShaderType::Domain;
			case D3D12_SHVER_COMPUTE_SHADER:
				return ShaderType::Compute;
			case D3D12_SHVER_MESH_SHADER:
				return ShaderType::Mesh;
			case D3D12_SHVER_AMPLIFICATION_SHADER:
				return ShaderType::Amplification;
			case D3D12_SHVER_RAY_GENERATION_SHADER:
				[[fallthrough]];
			case D3D12_SHVER_INTERSECTION_SHADER:
				[[fallthrough]];
			case D3D12_SHVER_ANY_HIT_SHADER:
				[[fallthrough]];
			case D3D12_SHVER_CLOSEST_HIT_SHADER:
				[[fallthrough]];
			case D3D12_SHVER_MISS_SHADER:
				[[fallthrough]];
			case D3D12_SHVER_CALLABLE_SHADER:
				return ShaderType::Compute;
			default:
				break;
			}

			return ShaderType::None;
		}

		template <typename ReflectionClass>
		static void ExtractResources( ReflectionClass& reflection, ShaderType bindingStage, uint32 numBoundResources, ShaderParameterMap& outParameterMap )
		{
			// Get the resources
			for ( uint32 i = 0; i < numBoundResources; ++i )
			{
				D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
				HRESULT hResult = reflection.GetResourceBindingDesc( i, &bindDesc );
				assert( SUCCEEDED( hResult ) );

				ShaderParameterType parameterType = ShaderParameterType::ConstantBuffer;
				uint32 parameterSize = 0;

				if ( bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER )
				{
					parameterType = ShaderParameterType::ConstantBuffer;

					ID3D12ShaderReflectionConstantBuffer* constBufferReflection = reflection.GetConstantBufferByName( bindDesc.Name );
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

							std::string_view paramName( shaderVarDesc.Name );
							bool isBindlessParam = paramName.starts_with( BindlessIndexTag );
							if ( isBindlessParam )
							{
								paramName.remove_prefix( BindlessIndexTag.length() );
							}

							ShaderParameterType paramType = isBindlessParam ? ShaderParameterType::Bindless : ShaderParameterType::ConstantBufferValue;

							outParameterMap.AddParameter( paramName.data(), bindingStage, paramType, bindDesc.BindPoint, bindDesc.Space, shaderVarDesc.StartOffset, shaderVarDesc.Size );
						}
					}
				}
				else if ( bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_RTACCELERATIONSTRUCTURE )
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

				outParameterMap.AddParameter( bindDesc.Name, bindingStage, parameterType, bindDesc.BindPoint, bindDesc.Space, 0, parameterSize );
			}
		}

		static void ExtractShaderParameters( ID3D12ShaderReflection& reflection, ShaderParameterMap& outParameterMap )
		{
			D3D12_SHADER_DESC shaderDesc = {};
			HRESULT hResult = reflection.GetDesc( &shaderDesc );
			assert( SUCCEEDED( hResult ) );

			ShaderType bindingStage = ResolveBindingStageFromVersion( shaderDesc.Version );

			ExtractResources( reflection, bindingStage, shaderDesc.BoundResources, outParameterMap );
		}

		static void ExtractShaderParameters( ID3D12LibraryReflection& reflection, ShaderParameterMap& outParameterMap )
		{
			D3D12_LIBRARY_DESC libraryDesc = {};
			HRESULT hResult = reflection.GetDesc( &libraryDesc );
			assert( SUCCEEDED( hResult ) );

			for ( uint32 functionIndex = 0; functionIndex < libraryDesc.FunctionCount; ++functionIndex )
			{
				ID3D12FunctionReflection* functionRefection = reflection.GetFunctionByIndex( functionIndex );

				D3D12_FUNCTION_DESC functionDesc;
				hResult = functionRefection->GetDesc( &functionDesc );
				assert( SUCCEEDED( hResult ) );

				ShaderType shaderType = ResolveBindingStageFromVersion( functionDesc.Version );

				ExtractResources( *functionRefection, shaderType, functionDesc.BoundResources, outParameterMap );
			}
		}
	};

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
		virtual void WaitQueue( QueueType type ) override;

		virtual LockedResource Lock( Buffer* buffer, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) override;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) override;

		virtual LockedResource Lock( Texture* texture, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) override;
		virtual void UnLock( Texture* texture, uint32 subResource = 0 ) override;

		virtual void GetRendererMultiSampleOption( MultiSampleOption* option ) override;

		virtual ICommandList* GetCommandList() override;
		virtual ICommandList* GetParallelCommandList() override;
		virtual IComputeCommandList* GetComputeCommandList() override;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const override;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const override;

		virtual std::filesystem::path GetShaderCacheFilePath() const override;

		virtual bool SupportsPSOCache() const override;
		virtual bool SupportsPSOLibraryCache() const override;
		virtual std::filesystem::path GetPSOCacheFilePath() const override;

		virtual bool SupportsHardwareRaytracing() const override;

		virtual bool SupportsMeshShader() const override;

		virtual bool SupportsWaveIntrinsics() const override;

		ID3D12Device8& GetDevice() const;
		IDXGIFactory7& GetFactory() const;
		ID3D12CommandQueue& GetDirectCommandQueue() const;
		ID3D12CommandQueue& GetComputeCommandQueue() const;

		D3D12ResourceAllocator& GetAllocator();
		D3D12QueryAllocator& GetQueryAllocator();
		D3D12CommnadListResourcePool& GetCmdPool( D3D12_COMMAND_LIST_TYPE type );
		D3D12ResourceUploader& GetUploader();
		D3D12BindlessManager& GetBindlessManager();
		D3D12FrameResourceCollection& GetFrameResourceCollection();
		D3D12ViewDescriptorPool& GetViewDescriptorPool();

		uint32 GetFrameIndex() const
		{
			return m_frameIndex;
		}

		virtual ~Direct3D12() override;

	private:
		bool CreateDeviceDependentResource();
		bool CreateDeviceIndependentResource();
		bool CheckFeatureSupport();

		const wchar_t* GetShaderProfile( ShaderType type ) const;

#ifdef _DEBUG
		ComPtr<ID3D12Debug1> m_debugLayer;
#endif

		ComPtr<IDXGIFactory7> m_factory;

		ComPtr<ID3D12Device8> m_device;
		ComPtr<ID3D12CommandQueue> m_directCommandQueue;
		ComPtr<ID3D12CommandQueue> m_computeCommandQueue;

		ComPtr<ID3D12Fence> m_fence;
		std::vector<uint64, InlineAllocator<uint64, 2>> m_fenceValues;
		uint64 m_lastFenceValue = 0;
		HANDLE m_fenceEvent = nullptr;

		ComPtr<ID3D12Fence> m_directFence;
		uint64 m_directFenceValue = 0;

		ComPtr<ID3D12Fence> m_computeFence;
		uint64 m_computeFenceValue = 0;

		ComPtr<IDxcCompiler3> m_compiler;
		ComPtr<IDxcLibrary> m_dxcLibrary;
		ComPtr<IDxcContainerReflection> m_reflection;

		bool m_psoLibraryCacheAvailable = false;
		bool m_raytracingAvailable = false;
		bool m_meshShaderAvailable = false;
		bool m_waveIntrinsicsAvailable = false;
		D3D12_FEATURE_DATA_SHADER_MODEL m_shaderModel = {};

		uint32 m_frameIndex = 0;

		std::vector<D3D12CommandList, InlineAllocator<D3D12CommandList, 2>> m_commandList;
		std::vector<D3D12ComputeCommandList, InlineAllocator<D3D12ComputeCommandList, 2>> m_computeCommandList;
		std::vector<D3D12FrameResourceCollection, InlineAllocator<D3D12FrameResourceCollection, 2>> m_frameResources;

		D3D12CommnadListResourcePool m_cmdListResourcePools[3] = {
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			D3D12_COMMAND_LIST_TYPE_COPY
		};

		D3D12ResourceAllocator m_allocator;
		D3D12QueryAllocator m_queryAllocator;
		D3D12ViewDescriptorPool m_viewDescriptorPool;

		D3D12ResourceUploader m_uploader;

		D3D12BindlessManager m_bindlessManager;
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

		if ( CheckFeatureSupport() == false )
		{
			return false;
		}

		return true;
	}

	void Direct3D12::OnShutdown()
	{
		m_uploader.WaitUntilUploadCompleted();
		m_uploader.WaitUntilCopyCompleted();

		// Dereferencing rendering resources
		for ( D3D12CommandList& commandList : m_commandList )
		{
			commandList.Prepare();
		}

		for ( D3D12ComputeCommandList& commandList : m_computeCommandList )
		{
			commandList.Prepare();
		}

		for ( auto& frameResourceCollection : m_frameResources )
		{
			frameResourceCollection.ReleaseResources();
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
		for ( auto& cmdPool : m_cmdListResourcePools )
		{
			cmdPool.Prepare();
		}

		m_uploader.Prepare();
		GetCommandList()->Prepare();
		GetComputeCommandList()->Prepare();

		D3D12FrameResources().ReleaseResources();

		auto& d3d12ResourceManager = *static_cast<D3D12ResourceManager*>( GetInterface<IResourceManager>() );
		d3d12ResourceManager.Prepare();
	}

	void Direct3D12::OnEndFrameRendering( uint32 curFrameIndex, uint32 nextFrameIndex )
	{
		if ( m_frameIndex == nextFrameIndex )
		{
			return;
		}

		uint64 fence = std::max( m_fenceValues[curFrameIndex], m_lastFenceValue );
		[[maybe_unused]] HRESULT hr = m_directCommandQueue->Signal( m_fence.Get(), fence );
		assert( SUCCEEDED( hr ) );

		uint64 nextFence = m_fenceValues[nextFrameIndex];
		if ( m_fence->GetCompletedValue() < nextFence )
		{
			hr = m_fence->SetEventOnCompletion( nextFence, m_fenceEvent );
			assert( SUCCEEDED( hr ) );
			WaitForSingleObject( m_fenceEvent, INFINITE );
		}

		m_lastFenceValue = m_fenceValues[nextFrameIndex] = fence + 1;
		m_frameIndex = nextFrameIndex;
	}

	void Direct3D12::WaitGPU()
	{
		assert( IsInRenderThread() );

		uint64 fence = m_fenceValues[m_frameIndex];
		[[maybe_unused]] HRESULT hr = m_directCommandQueue->Signal( m_fence.Get(), fence );
		assert( SUCCEEDED( hr ) );

		if ( m_fence->GetCompletedValue() < fence )
		{
			hr = m_fence->SetEventOnCompletion( fence, m_fenceEvent );
			assert( SUCCEEDED( hr ) );
			WaitForSingleObject( m_fenceEvent, INFINITE );
		}

		m_lastFenceValue = ++m_fenceValues[m_frameIndex];
	}

	void Direct3D12::WaitQueue( QueueType type )
	{
		GetCommandList()->Commit();
		GetComputeCommandList()->Commit();

		if ( type == QueueType::Direct )
		{
			GetDirectCommandQueue().Signal( m_directFence.Get(), m_directFenceValue );
			GetComputeCommandQueue().Wait( m_directFence.Get(), m_directFenceValue );
			++m_directFenceValue;
		}
		else // QueueType::Compute
		{
			GetComputeCommandQueue().Signal( m_computeFence.Get(), m_computeFenceValue );
			GetDirectCommandQueue().Wait( m_computeFence.Get(), m_computeFenceValue );
			++m_computeFenceValue;
		}
	}

	LockedResource Direct3D12::Lock( Buffer* buffer, ResourceLockFlag lockFlag, uint32 subResource )
	{
		auto d3d12Buffer = static_cast<D3D12Buffer*>( buffer );
		if ( d3d12Buffer == nullptr )
		{
			return {};
		}

		return d3d12Buffer->Lock( subResource, lockFlag );
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

	LockedResource Direct3D12::Lock( Texture* texture, [[maybe_unused]] ResourceLockFlag lockFlag, uint32 subResource )
	{
		auto d3d12Texture = static_cast<D3D12Texture*>( texture );
		if ( d3d12Texture == nullptr )
		{
			return {};
		}

		return d3d12Texture->Lock( subResource );
	}

	void Direct3D12::UnLock( Texture* texture, uint32 subResource )
	{
		auto d3d12Texture = static_cast<D3D12Texture*>( texture );
		if ( d3d12Texture == nullptr )
		{
			return;
		}

		return d3d12Texture->UnLock( subResource );
	}

	void Direct3D12::GetRendererMultiSampleOption( [[maybe_unused]] MultiSampleOption* option )
	{
	}

	ICommandList* Direct3D12::GetCommandList()
	{
		return &m_commandList[m_frameIndex];
	}

	ICommandList* Direct3D12::GetParallelCommandList()
	{
		return &m_commandList[m_frameIndex].GetParallelCommandList();
	}

	IComputeCommandList* Direct3D12::GetComputeCommandList()
	{
		return &m_computeCommandList[m_frameIndex];
	}

	BinaryChunk Direct3D12::CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const
	{
		DxcBuffer buffer = {
			.Ptr = source.Data(),
			.Size = source.Size(),
			.Encoding = DXC_CP_ACP
		};

		std::vector<const wchar_t*> args;
		args.reserve( 64 );

		// entry point
		if ( IsRaytracingShader( type ) )
		{
			args.push_back( L"-auto-binding-space 0" );
			args.push_back( L"-exports" );
		}
		else
		{
			args.push_back( L"-E" );
		}

		wchar_t wEntryPoint[64] = {};
		{
			ToWideChar( wEntryPoint, std::extent_v<decltype( wEntryPoint )>, entryPoint );
		}
		args.push_back( wEntryPoint );

		// target profile
		args.push_back( L"-T" );
		args.push_back( GetShaderProfile( type ) );

#if _DEBUG
		args.push_back( L"-Zs" );
#endif

		// defines
		args.push_back( L"-D" );
		args.push_back( L"D3D12=1");

		constexpr int32 MaxDefineLen = 256;
		std::vector<std::array<wchar_t, MaxDefineLen>> defineStorage;
		defineStorage.reserve( defines.size() / 2 );

		for ( uint32 i = 0; i < defines.size(); i += 2 )
		{
			if ( defines[i] == nullptr || defines[i + 1] == nullptr )
			{
				continue;
			}

			std::array<char, MaxDefineLen> define;

			SPrintf( define.data(), MaxDefineLen, "%s=%s", defines[i], defines[i + 1] );

			defineStorage.emplace_back();
			std::array<wchar_t, MaxDefineLen>& wDefine = defineStorage.back();

			ToWideChar( wDefine.data(), MaxDefineLen, define.data() );

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
			ComPtr<ID3D12LibraryReflection> libraryReflection;
			hr = m_reflection->GetPartReflection( shaderIndex, IID_PPV_ARGS( libraryReflection.GetAddressOf() ) );

			if ( FAILED( hr ) )
			{
				assert( false && "GetPartReflection was Failed" );
				return false;
			}

			D3D12ShaderReflectionLibrary::ExtractShaderParameters( *libraryReflection.Get(), outParameterMap );
		}
		else
		{
			D3D12ShaderReflectionLibrary::ExtractShaderParameters( *shaderReflection.Get(), outParameterMap );
		}

		BuildShaderParameterInfo( outParameterMap.GetParameterMap(), outParameterInfo );

		return true;
	}

	std::filesystem::path Direct3D12::GetShaderCacheFilePath() const
	{
		static auto shaderCacheFilePath = engine::Paths::GetSaveDir() / "ShaderCache-d3d12.asset";
		return shaderCacheFilePath;
	}

	bool Direct3D12::SupportsPSOCache() const
	{
		return true;
	}

	bool Direct3D12::SupportsPSOLibraryCache() const
	{
		return m_psoLibraryCacheAvailable;
	}

	std::filesystem::path Direct3D12::GetPSOCacheFilePath() const
	{
		static auto psoCacheFilePath = engine::Paths::GetSaveDir() / "PSOCache-d3d12.asset";
		return psoCacheFilePath;
	}

	bool Direct3D12::SupportsHardwareRaytracing() const
	{
		return m_raytracingAvailable;
	}

	bool Direct3D12::SupportsMeshShader() const
	{
		return m_meshShaderAvailable;
	}

	bool Direct3D12::SupportsWaveIntrinsics() const
	{
		return m_waveIntrinsicsAvailable
			// for WaveMath intrinsics
			&& m_shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_5;
	}

	ID3D12Device8& Direct3D12::GetDevice() const
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

	ID3D12CommandQueue& Direct3D12::GetComputeCommandQueue() const
	{
		return *m_computeCommandQueue.Get();
	}

	D3D12ResourceAllocator& Direct3D12::GetAllocator()
	{
		return m_allocator;
	}

	D3D12QueryAllocator& Direct3D12::GetQueryAllocator()
	{
		return m_queryAllocator;
	}

	D3D12CommnadListResourcePool& Direct3D12::GetCmdPool( D3D12_COMMAND_LIST_TYPE type )
	{
		switch ( type )
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return m_cmdListResourcePools[0];
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return m_cmdListResourcePools[1];
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return m_cmdListResourcePools[2];
		default:
			break;
		}

		assert( false );
		return m_cmdListResourcePools[0];
	}

	D3D12ResourceUploader& Direct3D12::GetUploader()
	{
		return m_uploader;
	}

	D3D12BindlessManager& Direct3D12::GetBindlessManager()
	{
		return m_bindlessManager;
	}

	D3D12FrameResourceCollection& Direct3D12::GetFrameResourceCollection()
	{
		return m_frameResources[m_frameIndex];
	}

	D3D12ViewDescriptorPool& Direct3D12::GetViewDescriptorPool()
	{
		return m_viewDescriptorPool;
	}

	Direct3D12::~Direct3D12()
	{
		CloseHandle( m_fenceEvent );
	}

	bool Direct3D12::CreateDeviceDependentResource()
	{
		D3D_FEATURE_LEVEL d3dFeatureLevel[] = {
			D3D_FEATURE_LEVEL_12_2,
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
		m_debugLayer->SetEnableGPUBasedValidation( DefaultAgl::IsGpuValidationEnabled() );
#endif

		std::vector<ComPtr<IDXGIAdapter1>> adapters;
		{
			assert( m_factory != nullptr );

			uint32 i = 0;
			ComPtr<IDXGIAdapter1> adapter;
			while ( m_factory->EnumAdapters1( i, adapter.GetAddressOf() ) != DXGI_ERROR_NOT_FOUND )
			{
				adapters.push_back( adapter );
				++i;
			}
		}

		ComPtr<IDXGIAdapter1> properAdapter;
		for ( ComPtr<IDXGIAdapter1> adapter : adapters )
		{
			DXGI_ADAPTER_DESC1 desc = {};
			adapter->GetDesc1( &desc );

			if ( IsAdapterIntegrated( adapter.Get() ) )
			{
				continue;
			}

			properAdapter = adapter;
			break;
		}

		for ( uint32 i = 0; i < _countof( d3dFeatureLevel ); ++i )
		{
			hr = D3D12CreateDevice( properAdapter.Get()
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

		D3D12_COMMAND_QUEUE_DESC directQueueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		hr = m_device->CreateCommandQueue( &directQueueDesc, IID_PPV_ARGS( m_directCommandQueue.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {
			.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};

		hr = m_device->CreateCommandQueue( &computeQueueDesc, IID_PPV_ARGS( m_computeCommandQueue.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_device->CreateFence( m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_fence ) );
		if ( FAILED( hr ) )
		{
			return false;
		}
		m_lastFenceValue = ++m_fenceValues[m_frameIndex];

		hr = m_device->CreateFence( m_directFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_directFence ) );
		if ( FAILED( hr ) )
		{
			return false;
		}
		++m_directFenceValue;

		hr = m_device->CreateFence( m_computeFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_computeFence ) );
		if ( FAILED( hr ) )
		{
			return false;
		}
		++m_computeFenceValue;

		m_commandList.resize( DefaultAgl::GetBufferCount() );
		for ( D3D12CommandList& commandList : m_commandList )
		{
			commandList.Initialize();
		}

		m_computeCommandList.resize( DefaultAgl::GetBufferCount() );
		for ( D3D12ComputeCommandList& commandList : m_computeCommandList )
		{
			commandList.Initialize();
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

		m_fenceValues.resize( DefaultAgl::GetBufferCount(), 0 );
		m_frameResources.resize( DefaultAgl::GetBufferCount() );

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

	bool Direct3D12::CheckFeatureSupport()
	{
		D3D12_FEATURE_DATA_SHADER_CACHE shaderCacheFeature = {};
		HRESULT hr = m_device->CheckFeatureSupport( D3D12_FEATURE_SHADER_CACHE, &shaderCacheFeature, sizeof( shaderCacheFeature ) );

		if ( FAILED( hr ) )
		{
			return false;
		}

		m_psoLibraryCacheAvailable = shaderCacheFeature.SupportFlags & D3D12_SHADER_CACHE_SUPPORT_LIBRARY;

		D3D12_FEATURE_DATA_D3D12_OPTIONS1 featureOptions1 = {};
		hr = m_device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS1, &featureOptions1, sizeof( featureOptions1 ) );

		if ( FAILED( hr ) )
		{
			return false;
		}

		m_waveIntrinsicsAvailable = featureOptions1.WaveOps;

		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureOption5 = {};
		hr = m_device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS5, &featureOption5, sizeof( featureOption5 ) );

		if ( FAILED( hr ) )
		{
			return false;
		}

		m_raytracingAvailable = ( featureOption5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED );

		D3D12_FEATURE_DATA_D3D12_OPTIONS7 featureOption7 = {};
		hr = m_device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS7, &featureOption7, sizeof( featureOption7 ) );

		if ( FAILED( hr ) )
		{
			return false;
		}

		m_meshShaderAvailable = ( featureOption7.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED );

		D3D_SHADER_MODEL allModelVersions[] = {
			D3D_SHADER_MODEL_6_9,
			D3D_SHADER_MODEL_6_8,
			D3D_SHADER_MODEL_6_7,
			D3D_SHADER_MODEL_6_6,
			D3D_SHADER_MODEL_6_5,
			D3D_SHADER_MODEL_6_4,
			D3D_SHADER_MODEL_6_3,
			D3D_SHADER_MODEL_6_2,
			D3D_SHADER_MODEL_6_1,
			D3D_SHADER_MODEL_6_0,
			D3D_SHADER_MODEL_5_1
		};

		for ( D3D_SHADER_MODEL shaderModel : allModelVersions )
		{
			m_shaderModel.HighestShaderModel = shaderModel;
			hr = m_device->CheckFeatureSupport( D3D12_FEATURE_SHADER_MODEL, &m_shaderModel, sizeof( m_shaderModel ) );

			if ( FAILED( hr ) == false )
			{
				break;
			}
		}

		return true;
	}

	const wchar_t* Direct3D12::GetShaderProfile( ShaderType type ) const
	{
		if ( type == ShaderType::Vertex )
		{
			switch ( m_shaderModel.HighestShaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"vs_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"vs_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"vs_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"vs_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"vs_6_9";
			}
		}
		else if ( type == ShaderType::Geometry )
		{
			switch ( m_shaderModel.HighestShaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"gs_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"gs_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"gs_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"gs_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"gs_6_9";
			}
		}
		else if ( type == ShaderType::Pixel )
		{
			switch ( m_shaderModel.HighestShaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"ps_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"ps_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"ps_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"ps_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"ps_6_9";
			}
		}
		else if ( type == ShaderType::Compute )
		{
			switch ( m_shaderModel.HighestShaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"cs_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"cs_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"cs_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"cs_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"cs_6_9";
			}
		}
		else if ( type == ShaderType::Amplification )
		{
			switch ( m_shaderModel.HighestShaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"as_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"as_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"as_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"as_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"as_6_9";
			}
		}
		else if ( type == ShaderType::Mesh )
		{
			switch ( m_shaderModel.HighestShaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"ms_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"ms_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"ms_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"ms_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"ms_6_9";
			}
		}
		else if ( IsRaytracingShader( type ) )
		{
			switch ( m_shaderModel.HighestShaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"lib_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"lib_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"lib_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"lib_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"lib_6_9";
			}
		}

		assert( false && "Invalid shader type" );
		return L"";
	}

	ID3D12CommandQueue& D3D12DirectCommandQueue()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetDirectCommandQueue();
	}

	ID3D12CommandQueue& D3D12ComputeCommandQueue()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetComputeCommandQueue();
	}

	ID3D12Device8& D3D12Device()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetDevice();
	}

	IDXGIFactory7& D3D12Factory()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetFactory();
	}

	D3D12ResourceAllocator& D3D12Allocator()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetAllocator();
	}

	D3D12QueryAllocator& D3D12AllocatorForQuery()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetQueryAllocator();
	}

	D3D12CommnadListResourcePool& D3D12CmdPool( D3D12_COMMAND_LIST_TYPE type )
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetCmdPool( type );
	}

	D3D12ResourceUploader& D3D12Uploader()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetUploader();
	}

	D3D12BindlessManager& D3D12BindlessMgr()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetBindlessManager();
	}

	D3D12FrameResourceCollection& D3D12FrameResources()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetFrameResourceCollection();
	}

	D3D12ViewDescriptorPool& D3D12DescriptorPoolForView()
	{
		auto d3d12Api = static_cast<Direct3D12*>( GetInterface<IAgl>() );
		return d3d12Api->GetViewDescriptorPool();
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
