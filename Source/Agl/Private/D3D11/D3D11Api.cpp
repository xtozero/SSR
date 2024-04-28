#include "D3D11Api.h"

#include "common.h"

#include "D3D11BlendState.h"
#include "D3D11Buffer.h"
#include "D3D11CommandList.h"
#include "D3D11DepthStencilState.h"
#include "D3D11FlagConvertor.h"
#include "D3D11RasterizerState.h"
#include "D3D11ResourceManager.h"
#include "D3D11ResourceViews.h"
#include "D3D11SamplerState.h"
#include "D3D11Shaders.h"
#include "D3D11Texture.h"
#include "D3D11VetexLayout.h"
#include "D3D11Viewport.h"

#include "EnumStringMap.h"

#include "IAgl.h"

#include "PipelineState.h"

#include "ShaderParameterMap.h"

#include "Texture.h"

#include <array>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace
{
	using ::agl::ShaderType;

	ShaderType ConvertShaderVersionToType( uint32 shaderVersion )
	{
		D3D11_SHADER_VERSION_TYPE shaderType = static_cast<D3D11_SHADER_VERSION_TYPE>( D3D11_SHVER_GET_TYPE( shaderVersion ) );

		switch ( shaderType )
		{
		case D3D11_SHVER_PIXEL_SHADER:
			return ShaderType::PS;
			break;
		case D3D11_SHVER_VERTEX_SHADER:
			return ShaderType::VS;
			break;
		case D3D11_SHVER_GEOMETRY_SHADER:
			return ShaderType::GS;
			break;
		case D3D11_SHVER_HULL_SHADER:
			return ShaderType::HS;
			break;
		case D3D11_SHVER_DOMAIN_SHADER:
			return ShaderType::DS;
			break;
		case D3D11_SHVER_COMPUTE_SHADER:
			return ShaderType::CS;
			break;
		default:
			break;
		}

		return ShaderType::None;
	}
}

namespace agl
{
	void ExtractShaderParameters( ID3D11ShaderReflection* pReflector, ShaderParameterMap& parameterMap )
	{
		D3D11_SHADER_DESC shaderDesc = {};
		HRESULT hResult = pReflector->GetDesc( &shaderDesc );
		assert( SUCCEEDED( hResult ) );

		ShaderType shaderType = ConvertShaderVersionToType( shaderDesc.Version );

		// Get the resources
		for ( uint32 i = 0; i < shaderDesc.BoundResources; ++i )
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
			hResult = pReflector->GetResourceBindingDesc( i, &bindDesc );
			assert( SUCCEEDED( hResult ) );

			ShaderParameterType parameterType = ShaderParameterType::ConstantBuffer;
			uint32 parameterSize = 0;

			if ( bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER )
			{
				parameterType = ShaderParameterType::ConstantBuffer;

				ID3D11ShaderReflectionConstantBuffer* constBufferReflection = pReflector->GetConstantBufferByName( bindDesc.Name );
				if ( constBufferReflection )
				{
					D3D11_SHADER_BUFFER_DESC shaderBuffDesc;
					constBufferReflection->GetDesc( &shaderBuffDesc );

					parameterSize = shaderBuffDesc.Size;

					for ( uint32 j = 0; j < shaderBuffDesc.Variables; ++j )
					{
						ID3D11ShaderReflectionVariable* variableReflection = constBufferReflection->GetVariableByIndex( j );
						D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
						variableReflection->GetDesc( &shaderVarDesc );

						parameterMap.AddParameter( shaderVarDesc.Name, shaderType, ShaderParameterType::ConstantBufferValue, bindDesc.BindPoint, 0, shaderVarDesc.StartOffset, shaderVarDesc.Size );
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

			parameterMap.AddParameter( bindDesc.Name, shaderType, parameterType, bindDesc.BindPoint, 0, 0, parameterSize );
		}
	}

	class CDirect3D11 final : public IAgl
	{
	public:
		virtual AglType GetType() const override;

		virtual bool BootUp() override;
		virtual void OnShutdown() override {}

		virtual void HandleDeviceLost() override;
		virtual void AppSizeChanged() override;
		virtual void OnBeginFrameRendering() override;
		virtual void OnEndFrameRendering( [[maybe_unused]] uint32 oldFrameIndex, [[maybe_unused]] uint32 newFrameIndex ) override {}
		virtual void WaitGPU() override;

		virtual LockedResource Lock( Buffer* buffer, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard, uint32 subResource = 0 ) override;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) override;

		virtual void GetRendererMultiSampleOption( MultiSampleOption* option ) override;

		virtual ICommandList* GetCommandList() override;
		virtual ICommandList* GetParallelCommandList() override;

		virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const override;
		virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const override;

		virtual const char* GetShaderCacheFilePath() const override;

		virtual bool IsSupportsPSOCache() const override;
		virtual const char* GetPSOCacheFilePath() const override;

		IDXGIFactory7& GetFactory() const
		{
			return *m_pdxgiFactory.Get();
		}

		ID3D11Device& GetDevice() const
		{
			return *m_pd3d11Device.Get();
		}

		ID3D11DeviceContext& GetDeviceContext() const
		{
			return *m_pd3d11DeviceContext.Get();
		}

		CDirect3D11();
		virtual ~CDirect3D11() override;

	private:
		void Shutdown();

		bool CreateDeviceDependentResource();
		bool CreateDeviceIndependentResource();
		void ReportLiveDevice();
		void EnumerateSampleCountAndQuality( int32* size, DXGI_SAMPLE_DESC* pSamples );

		Microsoft::WRL::ComPtr<IDXGIFactory7> m_pdxgiFactory;

		Microsoft::WRL::ComPtr<ID3D11Device> m_pd3d11Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pd3d11DeviceContext;

		DXGI_SAMPLE_DESC m_multiSampleOption = { 
			.Count = 1,
			.Quality = 0
		};

		D3D11CommandList m_commandList;
	};

	AglType CDirect3D11::GetType() const
	{
		return AglType::D3D11;
	}

	bool CDirect3D11::BootUp()
	{
		if ( !CreateDeviceIndependentResource() )
		{
			return false;
		}

		if ( !CreateDeviceDependentResource() )
		{
			return false;
		}

		return true;
	}

	void CDirect3D11::HandleDeviceLost()
	{
		m_pdxgiFactory.Reset();
		m_pd3d11DeviceContext.Reset();
		m_pd3d11Device.Reset();

		ReportLiveDevice();

		if ( !CreateDeviceIndependentResource() )
		{
			__debugbreak();
		}

		if ( !CreateDeviceDependentResource() )
		{
			__debugbreak();
		}
	}

	void CDirect3D11::AppSizeChanged()
	{
		if ( m_pd3d11Device == nullptr )
		{
			__debugbreak();
		}
	}

	void CDirect3D11::OnBeginFrameRendering()
	{
		m_commandList.Prepare();
	}

	void CDirect3D11::Shutdown()
	{
#ifdef _DEBUG
		ReportLiveDevice();
#endif
	}

	void CDirect3D11::WaitGPU()
	{
		assert( IsInRenderThread() );

		D3D11_QUERY_DESC desc = {
			.Query = D3D11_QUERY_EVENT,
			.MiscFlags = 0U
		};

		Microsoft::WRL::ComPtr<ID3D11Query> pQuery = nullptr;
		if ( SUCCEEDED( m_pd3d11Device->CreateQuery( &desc, pQuery.GetAddressOf() ) ) )
		{
			m_pd3d11DeviceContext->End( pQuery.Get() );
			BOOL data = 0;
			while ( m_pd3d11DeviceContext->GetData( pQuery.Get(), &data, sizeof( data ), 0 ) != S_OK ) {}
		}
	}

	LockedResource CDirect3D11::Lock( Buffer* buffer, ResourceLockFlag lockFlag, uint32 subResource )
	{
		if ( buffer == nullptr )
		{
			return {};
		}

		auto d3d11buffer = static_cast<D3D11Buffer*>( buffer );
		D3D11_MAPPED_SUBRESOURCE resource;

		HRESULT hr = m_pd3d11DeviceContext->Map( d3d11buffer->Resource(), subResource, ConvertLockFlagToD3D11Map( lockFlag ), 0, &resource );
		if ( FAILED( hr ) )
		{
			__debugbreak();
		}

		LockedResource result = {
			.m_data = resource.pData,
			.m_rowPitch = resource.RowPitch,
			.m_depthPitch = resource.DepthPitch
		};
		return result;
	}

	void CDirect3D11::UnLock( Buffer* buffer, uint32 subResource )
	{
		if ( buffer == nullptr )
		{
			return;
		}

		auto d3d11buffer = static_cast<D3D11Buffer*>( buffer );

		m_pd3d11DeviceContext->Unmap( d3d11buffer->Resource(), subResource );
	}

	void CDirect3D11::EnumerateSampleCountAndQuality( int32* size, DXGI_SAMPLE_DESC* pSamples )
	{
		assert( size != nullptr );

		constexpr int32 desireCounts[] = { 2, 4, 8 };
		uint32 qualityLevel = 0;
		for ( int32 i = 0; i < _countof( desireCounts ); ++i )
		{
			HRESULT hr = m_pd3d11Device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, desireCounts[i], &qualityLevel );
			if ( SUCCEEDED( hr ) && qualityLevel > 0 )
			{
				if ( pSamples != nullptr )
				{
					pSamples[i].Count = desireCounts[i];
					pSamples[i].Quality = qualityLevel - 1;
				}

				++( *size );
			}
		}
	}

	void CDirect3D11::GetRendererMultiSampleOption( MultiSampleOption* option )
	{
		assert( option != nullptr );
		option->m_count = m_multiSampleOption.Count;
		option->m_quality = m_multiSampleOption.Quality;
	}

	ICommandList* CDirect3D11::GetCommandList()
	{
		return &m_commandList;
	}

	ICommandList* CDirect3D11::GetParallelCommandList()
	{
		return &m_commandList.GetParallelCommandList();
	}

	BinaryChunk CDirect3D11::CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const
	{
		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorMsg = nullptr;

		std::vector<D3D_SHADER_MACRO> macros;

		macros.resize( ( defines.size() >> 1 ) + 1 );
		D3D_SHADER_MACRO* macro = macros.data();
		for ( uint32 i = 0; i < defines.size(); )
		{
			macro->Name = defines[i++];
			macro->Definition = defines[i++];
			++macro;
		}
		macros.back().Name = nullptr;
		macros.back().Definition = nullptr;

		HRESULT result = D3DCompile( source.Data(),
			source.Size(),
			nullptr,
			macros.data(),
			nullptr,
			"main",
			profile,
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&byteCode,
			&errorMsg );

		if ( SUCCEEDED( result ) )
		{
			BinaryChunk compiled( static_cast<uint32>( byteCode->GetBufferSize() ) );
			std::memcpy( compiled.Data(), byteCode->GetBufferPointer(), byteCode->GetBufferSize() );
			return compiled;
		}

		return {};
	}

	bool CDirect3D11::BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pShaderReflection = nullptr;
		HRESULT hr = D3DReflect( byteCode.Data(), byteCode.Size(), IID_PPV_ARGS(&pShaderReflection));
		if ( FAILED( hr ) )
		{
			return false;
		}

		ExtractShaderParameters( pShaderReflection.Get(), outParameterMap );
		BuildShaderParameterInfo( outParameterMap.GetParameterMap(), outParameterInfo );

		return true;
	}

	const char* CDirect3D11::GetShaderCacheFilePath() const
	{
		return "./Assets/Shaders/ShaderCache-d3d11.asset";
	}

	bool CDirect3D11::IsSupportsPSOCache() const
	{
		return false;
	}

	const char* CDirect3D11::GetPSOCacheFilePath() const
	{
		return "";
	}

	CDirect3D11::CDirect3D11()
	{
	}

	CDirect3D11::~CDirect3D11()
	{
		Shutdown();
	}

	bool CDirect3D11::CreateDeviceDependentResource()
	{
		D3D_DRIVER_TYPE d3dDriverTypes[] = {
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE
		};

		D3D_FEATURE_LEVEL d3dFeatureLevel[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		uint32 flag = 0;
#ifdef _DEBUG
		flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL selectedFeature = D3D_FEATURE_LEVEL_11_1;
		HRESULT hr = E_FAIL;

		for ( uint32 i = 0; i < _countof( d3dDriverTypes ); ++i )
		{
			hr = D3D11CreateDevice( nullptr,
				d3dDriverTypes[i],
				nullptr, flag,
				d3dFeatureLevel,
				_countof( d3dFeatureLevel ),
				D3D11_SDK_VERSION,
				m_pd3d11Device.GetAddressOf(),
				&selectedFeature,
				m_pd3d11DeviceContext.GetAddressOf()
			);

			if ( SUCCEEDED( hr ) )
			{
				// TODO: Viewport 에서 멀티 샘플링 관련 기능 구현
				/*int32 desiredSampleCount = 0;
				EnumerateSampleCountAndQuality( &desiredSampleCount, nullptr );

				std::vector<DXGI_SAMPLE_DESC> sampleCountAndQuality;
				sampleCountAndQuality.resize( desiredSampleCount );

				EnumerateSampleCountAndQuality( &desiredSampleCount, sampleCountAndQuality.data( ) );

				auto found = std::find_if( sampleCountAndQuality.begin( ), sampleCountAndQuality.end( ),
										[]( DXGI_SAMPLE_DESC& desc )
										{
											return desc.Count == 1;
										} );

				if ( found != sampleCountAndQuality.end( ) )
				{
					m_multiSampleOption = *found;
				}

				DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc = {};

				dxgiSwapchainDesc.BufferCount = 1;
				dxgiSwapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				dxgiSwapchainDesc.BufferDesc.Height = nWndHeight;
				dxgiSwapchainDesc.BufferDesc.Width = nWndWidth;
				dxgiSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
				dxgiSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
				dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				dxgiSwapchainDesc.OutputWindow = hWnd;
				dxgiSwapchainDesc.SampleDesc.Count = m_multiSampleOption.Count;
				dxgiSwapchainDesc.SampleDesc.Quality = m_multiSampleOption.Quality;
				dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				dxgiSwapchainDesc.Windowed = true;
				dxgiSwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

				hr = m_pdxgiFactory->CreateSwapChain( m_pd3d11Device.Get( ), &dxgiSwapchainDesc, m_pdxgiSwapChain.GetAddressOf( ) );
				if ( FAILED( hr ) )
				{
					return false;
				}

				m_resourceManager.OnDeviceRestore( m_pd3d11Device.Get( ), m_pd3d11DeviceContext.Get( ) );

				Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
				hr = m_pdxgiSwapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) );
				if ( FAILED( hr ) )
				{
					return false;
				}

				m_backBuffer = m_resourceManager.AddTexture2D( backBuffer, true );*/

				std::destroy_at( &m_commandList );
				std::construct_at( &m_commandList );
				m_commandList.Initialize();

				return true;
			}
		}

		return false;
	}

	bool CDirect3D11::CreateDeviceIndependentResource()
	{
		Microsoft::WRL::ComPtr<IDXGIFactory2> factory;

		uint32 factoryFlag = 0;
#ifdef _DEBUG
		factoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif
		HRESULT hr = CreateDXGIFactory2( factoryFlag, IID_PPV_ARGS( factory.GetAddressOf() ) );

		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = factory.As( &m_pdxgiFactory );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void CDirect3D11::ReportLiveDevice()
	{
		if ( m_pd3d11Device == nullptr )
		{
			return;
		}

		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3D11Debug> pD3dDebug;

		hr = m_pd3d11Device.Get()->QueryInterface( IID_PPV_ARGS( &pD3dDebug ) );

		if ( SUCCEEDED( hr ) )
		{
			pD3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL );
		}
	}

	ID3D11Device& D3D11Device()
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetInterface<IAgl>() );
		return d3d11Api->GetDevice();
	}

	ID3D11DeviceContext& D3D11Context()
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetInterface<IAgl>() );
		return d3d11Api->GetDeviceContext();
	}

	IDXGIFactory7& D3D11Factory()
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetInterface<IAgl>() );
		return d3d11Api->GetFactory();
	}

	Owner<IAgl*> CreateD3D11GraphicsApi()
	{
		return new CDirect3D11();
	}
}