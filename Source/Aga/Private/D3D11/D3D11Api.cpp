#include "stdafx.h"
#include "D3D11Api.h"

#include "common.h"

#include "D3D11BaseTexture.h"
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
#include "D3D11VetexLayout.h"
#include "D3D11Viewport.h"

#include "EnumStringMap.h"

#include "IAga.h"

#include "PipelineState.h"

#include "ShaderParameterMap.h"

#include "Texture.h"

#include <array>
#include <d3d11.h>
#include <dxgi.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace aga
{
	IAga* g_AbstractGraphicsApi = nullptr;

	class CDirect3D11 : public IAga
	{
	public:
		virtual bool BootUp() override;
		virtual void HandleDeviceLost() override;
		virtual void AppSizeChanged() override;
		virtual void WaitGPU() override;

		virtual void* Lock( Buffer* buffer, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 ) override;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) override;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) override;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) override;

		virtual IImmediateCommandList* GetImmediateCommandList() override;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList() const override;

		IDXGIFactory1& GetFactory() const
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
		virtual ~CDirect3D11();

	private:
		void Shutdown();

		bool CreateDeviceDependentResource();
		bool CreateDeviceIndependentResource();
		void ReportLiveDevice();
		void EnumerateSampleCountAndQuality( int32* size, DXGI_SAMPLE_DESC* pSamples );

		Microsoft::WRL::ComPtr<IDXGIFactory1>					m_pdxgiFactory;

		Microsoft::WRL::ComPtr<ID3D11Device>					m_pd3d11Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pd3d11DeviceContext;

		DXGI_SAMPLE_DESC										m_multiSampleOption = { 1, 0 };

		D3D11ImmediateCommandList								m_immediateCommandList;
	};

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

	void CDirect3D11::Shutdown()
	{
#ifdef _DEBUG
		ReportLiveDevice();
#endif
	}

	void CDirect3D11::WaitGPU()
	{
		D3D11_QUERY_DESC desc = {
			D3D11_QUERY_EVENT,
			0U
		};

		Microsoft::WRL::ComPtr<ID3D11Query> pQuery = nullptr;
		if ( SUCCEEDED( m_pd3d11Device->CreateQuery( &desc, pQuery.GetAddressOf() ) ) )
		{
			m_pd3d11DeviceContext->End( pQuery.Get() );
			BOOL data = 0;
			while ( m_pd3d11DeviceContext->GetData( pQuery.Get(), &data, sizeof( data ), 0 ) != S_OK ) {}
		}
	}

	void* CDirect3D11::Lock( Buffer* buffer, uint32 lockFlag, uint32 subResource )
	{
		if ( buffer == nullptr )
		{
			return nullptr;
		}

		auto d3d11buffer = static_cast<D3D11Buffer*>( buffer );
		D3D11_MAPPED_SUBRESOURCE resource;

		HRESULT hr = m_pd3d11DeviceContext->Map( d3d11buffer->Resource(), subResource, ConvertLockFlagToD3D11Map( lockFlag ), 0, &resource );
		if ( FAILED( hr ) )
		{
			__debugbreak();
		}

		return resource.pData;
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

	void CDirect3D11::Copy( Buffer* dst, Buffer* src, uint32 size )
	{
		auto d3d11Dst = static_cast<D3D11Buffer*>( dst );
		auto d3d11Src = static_cast<D3D11Buffer*>( src );

		if ( ( d3d11Dst == nullptr ) || ( d3d11Src == nullptr ) )
		{
			return;
		}

		ID3D11Buffer* dstBuffer = d3d11Dst->Resource();
		ID3D11Buffer* srcBuffer = d3d11Src->Resource();

		if ( ( dstBuffer == nullptr ) || ( srcBuffer == nullptr ) )
		{
			return;
		}

		D3D11_BOX srcBox = {
			0,
			0,
			0,
			size,
			1,
			1,
		};

		m_pd3d11DeviceContext->CopySubresourceRegion( dstBuffer, 0, 0, 0, 0, srcBuffer, 0, &srcBox );
	}

	void CDirect3D11::GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option )
	{
		assert( option != nullptr );
		option->m_count = m_multiSampleOption.Count;
		option->m_quality = m_multiSampleOption.Quality;
	}

	IImmediateCommandList* CDirect3D11::GetImmediateCommandList()
	{
		return &m_immediateCommandList;
	}

	std::unique_ptr<IDeferredCommandList> CDirect3D11::CreateDeferredCommandList() const
	{
		return std::make_unique<D3D11DeferredCommandList>();
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
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		uint32 flag = 0;
#ifdef _DEBUG
		flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL selectedFeature = D3D_FEATURE_LEVEL_11_0;
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

				std::construct_at( &m_immediateCommandList );

				return true;
			}
		}

		return false;
	}

	bool CDirect3D11::CreateDeviceIndependentResource()
	{
		HRESULT hr = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast<void**>( m_pdxgiFactory.GetAddressOf() ) );
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
		auto d3d11Api = static_cast<CDirect3D11*>( GetD3D11GraphicsApi() );
		return d3d11Api->GetDevice();
	}

	ID3D11DeviceContext& D3D11Context()
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetD3D11GraphicsApi() );
		return d3d11Api->GetDeviceContext();
	}

	IDXGIFactory1& D3D11Factory()
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetD3D11GraphicsApi() );
		return d3d11Api->GetFactory();
	}

	void CreateD3D11GraphicsApi()
	{
		g_AbstractGraphicsApi = new CDirect3D11();
	}

	void DestoryD3D11GraphicsApi()
	{
		delete g_AbstractGraphicsApi;
	}

	void* GetD3D11GraphicsApi()
	{
		return g_AbstractGraphicsApi;
	}
}