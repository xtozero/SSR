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

#include "ShaderPrameterMap.h"

#include "Texture.h"

#include <array>
#include <d3d11.h>
#include <dxgi.h>
#include <map>
#include <string>
#include <vector>

namespace aga
{
	IAga* g_AbstractGraphicsApi = nullptr;

	class CDirect3D11 : public IAga
	{
	public:
		virtual bool BootUp( ) override;
		virtual void HandleDeviceLost( ) override;
		virtual void AppSizeChanged( ) override;
		virtual void WaitGPU( ) override;

		virtual void* Lock( Buffer* buffer, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 ) override;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) override;

		virtual void SetViewports( Viewport** viewPorts, uint32 count ) override;
		virtual void SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ ) override;
		virtual void SetScissorRects( Viewport** viewPorts, uint32 size ) override;
		virtual void SetScissorRect( uint32 minX, uint32 minY, uint32 maxX, uint32 maxY ) override;

		virtual void ClearDepthStencil( Texture* depthStencil, float depthColor, uint8 stencilColor ) override;

		virtual void BindShader( ComputeShader* shader ) override;

		virtual void BindConstant( VertexShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) override;
		virtual void BindShaderInput( VertexShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) override;

		virtual void BindConstant( PixelShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) override;
		virtual void BindShaderInput( PixelShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) override;

		virtual void BindConstant( ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) override;
		virtual void BindShaderInput( ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) override;
		virtual void BindShaderOutput( ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) override;

		virtual void BindRenderTargets( Texture** pRenderTargets, uint32 renderTargetCount, Texture* depthStencil ) override;

		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) override;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) override;

		virtual std::unique_ptr<IImmediateCommandList> GetImmediateCommandList( ) const override;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList( ) const override;

		IDXGIFactory1& GetFactory( ) const
		{
			return *m_pdxgiFactory.Get( );
		}

		ID3D11Device& GetDevice( ) const
		{
			return *m_pd3d11Device.Get( );
		}

		ID3D11DeviceContext& GetDeviceContext( ) const
		{
			return *m_pd3d11DeviceContext.Get( );
		}

		CDirect3D11( );
		virtual ~CDirect3D11( );

	private:
		void Shutdown( );

		bool CreateDeviceDependentResource( );
		bool CreateDeviceIndependentResource( );
		void ReportLiveDevice( );
		void EnumerateSampleCountAndQuality( int32* size, DXGI_SAMPLE_DESC* pSamples );

		Microsoft::WRL::ComPtr<IDXGIFactory1>					m_pdxgiFactory;

		Microsoft::WRL::ComPtr<ID3D11Device>					m_pd3d11Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pd3d11DeviceContext;

		DXGI_SAMPLE_DESC										m_multiSampleOption = { 1, 0 };
	};

	bool CDirect3D11::BootUp( )
	{
		if ( !CreateDeviceIndependentResource( ) )
		{
			return false;
		}

		if ( !CreateDeviceDependentResource( ) )
		{
			return false;
		}

		return true;
	}

	void CDirect3D11::HandleDeviceLost( )
	{
		m_pdxgiFactory.Reset( );
		m_pd3d11DeviceContext.Reset( );
		m_pd3d11Device.Reset( );

		ReportLiveDevice( );

		if ( !CreateDeviceIndependentResource( ) )
		{
			__debugbreak( );
		}

		if ( !CreateDeviceDependentResource( ) )
		{
			__debugbreak( );
		}
	}

	void CDirect3D11::AppSizeChanged( )
	{
		if ( m_pd3d11Device == nullptr )
		{
			__debugbreak( );
		}
	}

	void CDirect3D11::Shutdown( )
	{
#ifdef _DEBUG
		ReportLiveDevice( );
#endif
	}

	void CDirect3D11::WaitGPU( )
	{
		D3D11_QUERY_DESC desc = {
			D3D11_QUERY_EVENT,
			0U
		};

		Microsoft::WRL::ComPtr<ID3D11Query> pQuery = nullptr;
		if ( SUCCEEDED( m_pd3d11Device->CreateQuery( &desc, pQuery.GetAddressOf( ) ) ) )
		{
			m_pd3d11DeviceContext->End( pQuery.Get( ) );
			BOOL data = 0;
			while ( m_pd3d11DeviceContext->GetData( pQuery.Get( ), &data, sizeof( data ), 0 ) != S_OK ) {}
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

		HRESULT hr = m_pd3d11DeviceContext->Map( d3d11buffer->Resource( ), subResource, ConvertLockFlagToD3D11Map( lockFlag ), 0, &resource );
		if ( FAILED( hr ) )
		{
			__debugbreak( );
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

		m_pd3d11DeviceContext->Unmap( d3d11buffer->Resource( ), subResource );
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

	void CDirect3D11::SetViewports( Viewport** viewPorts, uint32 size )
	{
		std::vector<D3D11_VIEWPORT> d3d11Viewports;

		for ( uint32 i = 0; i < size; ++i )
		{
			if ( auto vp = static_cast<const D3D11Viewport*>( viewPorts[i] ) )
			{
				auto renderTargetSize = vp->Size( );
				D3D11_VIEWPORT newVeiwport = {
					0.f,
					0.f,
					static_cast<float>( renderTargetSize.first ),
					static_cast<float>( renderTargetSize.second ),
					0.f,
					1.f };

				d3d11Viewports.push_back( newVeiwport );
			}
		}

		m_pd3d11DeviceContext->RSSetViewports( size, d3d11Viewports.data( ) );
	}

	void CDirect3D11::SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ )
	{
		D3D11_VIEWPORT viewport = {
			static_cast<float>( minX ),
			static_cast<float>( minY ),
			static_cast<float>( maxX ),
			static_cast<float>( maxY ),
			minZ,
			maxZ
		};

		m_pd3d11DeviceContext->RSSetViewports( 1, &viewport );
	}

	void CDirect3D11::SetScissorRects( Viewport** viewPorts, uint32 size )
	{
		std::vector<D3D11_RECT> d3d11Rects;

		for ( uint32 i = 0; i < size; ++i )
		{
			if ( auto vp = static_cast<const D3D11Viewport*>( viewPorts[i] ) )
			{
				auto renderTargetSize = vp->Size( );
				const auto& [width, height] = renderTargetSize;
				D3D11_RECT newVeiwport = { 0L, 0L, static_cast<int32>( width ), static_cast<int32>( height ) };

				d3d11Rects.push_back( newVeiwport );
			}
		}

		m_pd3d11DeviceContext->RSSetScissorRects( size, d3d11Rects.data( ) );
	}

	void CDirect3D11::SetScissorRect( uint32 minX, uint32 minY, uint32 maxX, uint32 maxY )
	{
		D3D11_RECT rect = {};
		rect.left = minX;
		rect.top = minY;
		rect.right = maxX;
		rect.bottom = maxY;

		m_pd3d11DeviceContext->RSSetScissorRects( 1, &rect );
	}

	void CDirect3D11::ClearDepthStencil( Texture* depthStencil, float depthColor, uint8 stencilColor )
	{
		auto d3d11Texture = reinterpret_cast<D3D11BaseTexture*>( depthStencil );
		if ( d3d11Texture == nullptr )
		{
			return;
		}

		if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( d3d11Texture->DSV( ) ) )
		{
			ID3D11DepthStencilView* dsv = d3d11DSV->Resource( );

			m_pd3d11DeviceContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
		}
	}

	void CDirect3D11::BindShader( ComputeShader* shader )
	{
		ID3D11ComputeShader* cs = nullptr;
		if ( auto d3d11CS = static_cast<D3D11ComputeShader*>( shader ) )
		{
			cs = d3d11CS->Resource( );
		}

		D3D11Context( ).CSSetShader( cs, nullptr, 0 );
	}

	void CDirect3D11::BindConstant( [[maybe_unused]] VertexShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers )
	{
		ID3D11Buffer* pConstants[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

		for ( uint32 i = 0; i < numBuffers; ++i )
		{
			if ( auto d3d11buffer = static_cast<D3D11Buffer*>( pBuffers[i] ) )
			{
				pConstants[i] = d3d11buffer->Resource( );
			}
		}

		m_pd3d11DeviceContext->VSSetConstantBuffers( startSlot, numBuffers, pConstants );
	}

	void CDirect3D11::BindShaderInput( [[maybe_unused]] VertexShader* shader, uint32 startSlot, uint32 numBuffers, Buffer ** pBuffers )
	{
		ID3D11ShaderResourceView* pSrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

		for ( uint32 i = 0; i < numBuffers; ++i )
		{
			auto d3d11buffer = static_cast<D3D11Buffer*>( pBuffers[i] );
			if ( d3d11buffer == nullptr )
			{
				continue;
			}

			if ( auto d3d11SRV = static_cast<D3D11ShaderResourceView*>( d3d11buffer->SRV( ) ) )
			{
				pSrvs[i] = d3d11SRV->Resource( );
			}
		}

		m_pd3d11DeviceContext->VSSetShaderResources( startSlot, numBuffers, pSrvs );
	}

	void CDirect3D11::BindConstant( [[maybe_unused]] PixelShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers )
	{
		ID3D11Buffer* pConstants[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

		for ( uint32 i = 0; i < numBuffers; ++i )
		{
			if ( auto d3d11buffer = static_cast<D3D11Buffer*>( pBuffers[i] ) )
			{
				pConstants[i] = d3d11buffer->Resource( );
			}
		}

		m_pd3d11DeviceContext->CSSetConstantBuffers( startSlot, numBuffers, pConstants );
	}

	void CDirect3D11::BindShaderInput( [[maybe_unused]] PixelShader* shader, uint32 startSlot, uint32 numBuffers, Buffer ** pBuffers )
	{
		ID3D11ShaderResourceView* pSrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

		for ( uint32 i = 0; i < numBuffers; ++i )
		{
			auto d3d11buffer = static_cast<D3D11Buffer*>( pBuffers[i] );
			if ( d3d11buffer == nullptr )
			{
				continue;
			}

			if ( auto d3d11SRV = static_cast<D3D11ShaderResourceView*>( d3d11buffer->SRV( ) ) )
			{
				pSrvs[i] = d3d11SRV->Resource( );
			}
		}

		m_pd3d11DeviceContext->CSSetShaderResources( startSlot, numBuffers, pSrvs );
	}

	void CDirect3D11::BindConstant( [[maybe_unused]] ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers )
	{
		ID3D11Buffer* pConstants[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

		for ( uint32 i = 0; i < numBuffers; ++i )
		{
			if ( auto d3d11buffer = static_cast<D3D11Buffer*>( pBuffers[i] ) )
			{
				pConstants[i] = d3d11buffer->Resource( );
			}
		}

		m_pd3d11DeviceContext->CSSetConstantBuffers( startSlot, numBuffers, pConstants );
	}

	void CDirect3D11::BindShaderInput( [[maybe_unused]] ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers )
	{
		ID3D11ShaderResourceView* pSrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

		for ( uint32 i = 0; i < numBuffers; ++i )
		{
			auto d3d11buffer = static_cast<D3D11Buffer*>( pBuffers[i] );
			if ( d3d11buffer == nullptr )
			{
				continue;
			}

			if ( auto d3d11SRV = static_cast<D3D11ShaderResourceView*>( d3d11buffer->SRV( ) ) )
			{
				pSrvs[i] = d3d11SRV->Resource( );
			}
		}

		m_pd3d11DeviceContext->CSSetShaderResources( startSlot, numBuffers, pSrvs );
	}

	void CDirect3D11::BindShaderOutput( [[maybe_unused]] ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers )
	{
		ID3D11UnorderedAccessView* pUavs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

		for ( uint32 i = 0; i < numBuffers; ++i )
		{
			auto d3d11buffer = static_cast<D3D11Buffer*>( pBuffers[i] );
			if ( d3d11buffer == nullptr )
			{
				continue;
			}

			if ( auto d3d11UAV = static_cast<D3D11UnorderedAccessView*>( d3d11buffer->UAV( ) ) )
			{
				pUavs[i] = d3d11UAV->Resource( );
			}
		}

		m_pd3d11DeviceContext->CSSetUnorderedAccessViews( startSlot, numBuffers, pUavs, nullptr );
	}

	void CDirect3D11::BindRenderTargets( Texture** pRenderTargets, uint32 renderTargetCount, Texture* depthStencil )
	{
		ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

		for ( uint32 i = 0; i < renderTargetCount; ++i )
		{
			auto rtTex = static_cast<D3D11BaseTexture*>( pRenderTargets[i] );
			if ( rtTex == nullptr )
			{
				continue;
			}

			if ( auto d3d11RTV = static_cast<D3D11RenderTargetView*>( rtTex->RTV( ) ) )
			{
				rtvs[i] = d3d11RTV->Resource( );
			}
		}

		ID3D11DepthStencilView* dsv = nullptr;

		if ( auto dsTex = static_cast<D3D11BaseTexture*>( depthStencil ) )
		{
			if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( dsTex->DSV( ) ) )
			{
				dsv = d3d11DSV->Resource( );
			}
		}

		m_pd3d11DeviceContext->OMSetRenderTargets( renderTargetCount, rtvs, dsv );
	}

	void CDirect3D11::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_pd3d11DeviceContext->Dispatch( x, y, z );
	}

	void CDirect3D11::Copy( Buffer* dst, Buffer* src, uint32 size )
	{
		auto d3d11Dst = static_cast<D3D11Buffer*>( dst );
		auto d3d11Src = static_cast<D3D11Buffer*>( src );

		if ( ( d3d11Dst == nullptr ) || ( d3d11Src == nullptr ) )
		{
			return;
		}

		ID3D11Buffer* dstBuffer = d3d11Dst->Resource( );
		ID3D11Buffer* srcBuffer = d3d11Src->Resource( );

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

	std::unique_ptr<IImmediateCommandList> CDirect3D11::GetImmediateCommandList( ) const
	{
		return std::make_unique<D3D11ImmediateCommandList>( );
	}

	std::unique_ptr<IDeferredCommandList> CDirect3D11::CreateDeferredCommandList( ) const
	{
		return std::make_unique<D3D11DeferredCommandList>( );
	}

	CDirect3D11::CDirect3D11( )
	{
	}

	CDirect3D11::~CDirect3D11( )
	{
		Shutdown( );
	}

	bool CDirect3D11::CreateDeviceDependentResource( )
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
				m_pd3d11Device.GetAddressOf( ),
				&selectedFeature,
				m_pd3d11DeviceContext.GetAddressOf( )
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

				return true;
			}
		}

		return false;
	}

	bool CDirect3D11::CreateDeviceIndependentResource( )
	{
		HRESULT hr = CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), reinterpret_cast<void**>( m_pdxgiFactory.GetAddressOf( ) ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void CDirect3D11::ReportLiveDevice( )
	{
		if ( m_pd3d11Device == nullptr )
		{
			return;
		}

		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3D11Debug> pD3dDebug;

		hr = m_pd3d11Device.Get( )->QueryInterface( IID_PPV_ARGS( &pD3dDebug ) );

		if ( SUCCEEDED( hr ) )
		{
			pD3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL );
		}
	}

	ID3D11Device& D3D11Device( )
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetD3D11GraphicsApi( ) );
		return d3d11Api->GetDevice( );
	}

	ID3D11DeviceContext& D3D11Context( )
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetD3D11GraphicsApi( ) );
		return d3d11Api->GetDeviceContext( );
	}

	IDXGIFactory1& D3D11Factory( )
	{
		auto d3d11Api = static_cast<CDirect3D11*>( GetD3D11GraphicsApi( ) );
		return d3d11Api->GetFactory( );
	}

	void CreateD3D11GraphicsApi( )
	{
		g_AbstractGraphicsApi = new CDirect3D11( );
	}

	void DestoryD3D11GraphicsApi( )
	{
		delete g_AbstractGraphicsApi;
	}

	void* GetD3D11GraphicsApi( )
	{
		return g_AbstractGraphicsApi;
	}
}