#include "stdafx.h"
#include "D3D11Api.h"

#include "common.h"
#include "Common/IAga.h"
#include "D3D11ResourceInterface.h"

#include "D3D11BlendState.h"
#include "D3D11Buffer.h"
#include "D3D11DepthStencil.h"
#include "D3D11DepthStencilState.h"
#include "D3D11FlagConvertor.h"
#include "D3D11RandomAccessResource.h"
#include "D3D11RasterizerState.h"
#include "D3D11RenderTarget.h"
#include "D3D11ResourceManager.h"
#include "D3D11SamplerState.h"
#include "D3D11ShaderResource.h"
#include "D3D11Shaders.h"
#include "D3D11VetexLayout.h"

#include "DataStructure/EnumStringMap.h"
#include "Util.h"

#include <array>
#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>
#include <map>
#include <string>
#include <vector>

namespace
{
	IAga* g_AbstractGraphicsApi = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Device> g_pd3d11Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_pd3d11DeviceContext;
};

class CDirect3D11 : public IAga
{
public:
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;
	virtual void WaitGPU( ) override;

	virtual RE_HANDLE CreateTexture1D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual RE_HANDLE CreateTexture2D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual RE_HANDLE CreateTexture3D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait ) override;

	virtual RE_HANDLE CreateVertexLayout( RE_HANDLE vsHandle, const VERTEX_LAYOUT* layoutOrNull, int layoutSize ) override;
	virtual RE_HANDLE CreateVertexShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;
	virtual RE_HANDLE CreateGeometryShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;
	virtual RE_HANDLE CreatePixelShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;
	virtual RE_HANDLE CreateComputeShader( const void* byteCodePtr, std::size_t byteCodeSize ) override;

	virtual RE_HANDLE CreateRenderTarget( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateDepthStencil( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;

	virtual RE_HANDLE CreateTexture1DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateTexture2DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateTexture3DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateBufferShaderResource( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) override;

	virtual RE_HANDLE CreateTexture1DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateTexture2DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateBufferRandomAccess( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) override;

	virtual RE_HANDLE CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateBlendState( const BLEND_STATE_TRAIT& trait ) override;

	virtual void* LockBuffer( RE_HANDLE buffer, int lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) override;
	virtual void UnLockBuffer( RE_HANDLE buffer, UINT subResource = 0 ) override;

	virtual void SetViewports( const Viewport* viewPorts, int count ) override;
	virtual void SetScissorRects( const RECT* rects, int size ) override;

	virtual void ClearRendertarget( RE_HANDLE renderTarget, const float( &clearColor )[4] ) override;
	virtual void ClearDepthStencil( RE_HANDLE depthStencil, float depthColor, UINT8 stencilColor ) override;

	virtual void BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets ) override;
	virtual void BindIndexBuffer( RE_HANDLE indexBuffer, UINT indexOffset ) override;
	virtual void BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, const RE_HANDLE* pConstantBuffers ) override;
	virtual void BindVertexLayout( RE_HANDLE layout ) override;
	virtual void BindShader( RE_HANDLE shader ) override;
	virtual void BindShaderResource( SHADER_TYPE type, int startSlot, int count, const RE_HANDLE* resource ) override;
	virtual void BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource ) override;
	virtual void BindRenderTargets( const RE_HANDLE* pRenderTargets, int renderTargetCount, RE_HANDLE depthStencil ) override;
	virtual void BindRasterizerState( RE_HANDLE rasterizerState ) override;
	virtual void BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, const RE_HANDLE* pSamplerStates ) override;
	virtual void BindDepthStencilState( RE_HANDLE depthStencilState ) override;
	virtual void BindBlendState( RE_HANDLE blendState ) override;

	virtual void Draw( UINT primitive, UINT vertexCount, UINT vertexOffset = 0 ) override;
	virtual void DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) override;
	virtual void DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawAuto( ) override;
	virtual void Dispatch( int x, int y, int z = 1 ) override;

	virtual BYTE Present( ) override;

	virtual void GenerateMips( RE_HANDLE shaderResource ) override;

	virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) override;

	CDirect3D11( );
	virtual ~CDirect3D11( );

private:
	void Shutdown( );

	bool CreateDeviceDependentResource( HWND hWnd, UINT nWndWidth, UINT nWndHeight );
	bool CreateDeviceIndependentResource( );
	void ReportLiveDevice( );
	void EnumerateSampleCountAndQuality( int* size, DXGI_SAMPLE_DESC* pSamples );

	Microsoft::WRL::ComPtr<IDXGIFactory1>					m_pdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain>					m_pdxgiSwapChain;

	CD3D11ResourceManager									m_resourceManager;

	DXGI_SAMPLE_DESC										m_multiSampleOption = { 1, 0 };
};

bool CDirect3D11::BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight )
{
	if ( !CreateDeviceIndependentResource( ) )
	{
		return false;
	}

	if ( !CreateDeviceDependentResource( hWnd, nWndWidth, nWndHeight ) )
	{
		return false;
	}

	if ( !m_resourceManager.Bootup( g_pd3d11Device.Get( ), g_pd3d11DeviceContext.Get( ) ) )
	{
		return false;
	}
	m_resourceManager.AppSizeChanged( nWndWidth, nWndHeight );

	return true;
}

void CDirect3D11::HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight )
{
	m_resourceManager.OnDeviceLost( );

	m_pdxgiSwapChain.Reset( );
	m_pdxgiFactory.Reset( );
	g_pd3d11DeviceContext.Reset( );
	g_pd3d11Device.Reset( );

	ReportLiveDevice( );

	if ( !CreateDeviceIndependentResource( ) )
	{
		__debugbreak( );
	}

	if ( !CreateDeviceDependentResource( hWnd, nWndWidth, nWndHeight ) )
	{
		__debugbreak( );
	}

	m_resourceManager.AppSizeChanged( nWndWidth, nWndHeight );
}

void CDirect3D11::AppSizeChanged( UINT nWndWidth, UINT nWndHeight )
{
	if ( g_pd3d11Device == nullptr )
	{
		__debugbreak( );
	}

	if ( m_pdxgiSwapChain == nullptr )
	{
		__debugbreak( );
	}

	m_resourceManager.AppSizeChanged( nWndWidth, nWndHeight );

	HRESULT hr = m_pdxgiSwapChain->ResizeBuffers( 1, nWndWidth, nWndHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
	if ( FAILED( hr ) )
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
	if ( SUCCEEDED( g_pd3d11Device->CreateQuery( &desc, pQuery.GetAddressOf( ) ) ) )
	{
		g_pd3d11DeviceContext->End( pQuery.Get( ) );
		BOOL data = 0;
		while( g_pd3d11DeviceContext->GetData( pQuery.Get( ), &data, sizeof( data ), 0 ) != S_OK ) { }
	}
}

RE_HANDLE CDirect3D11::CreateTexture1D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	return m_resourceManager.CreateTexture1D( trait, initData );
}

RE_HANDLE CDirect3D11::CreateTexture2D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	return m_resourceManager.CreateTexture2D( trait, initData );
}

RE_HANDLE CDirect3D11::CreateTexture3D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	return m_resourceManager.CreateTexture3D( trait, initData );
}

RE_HANDLE CDirect3D11::CreateBuffer( const BUFFER_TRAIT& trait )
{
	return m_resourceManager.CreateBuffer( trait );
}

RE_HANDLE CDirect3D11::CreateVertexLayout( RE_HANDLE vsHandle, const VERTEX_LAYOUT* layoutOrNull, int layoutSize )
{
	return m_resourceManager.CreateVertexLayout( vsHandle, layoutOrNull, layoutSize );
}

RE_HANDLE CDirect3D11::CreateVertexShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	return m_resourceManager.CreateVertexShader( byteCodePtr, byteCodeSize );
}

RE_HANDLE CDirect3D11::CreateGeometryShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	return m_resourceManager.CreateGeometryShader( byteCodePtr, byteCodeSize );
}

RE_HANDLE CDirect3D11::CreatePixelShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	return m_resourceManager.CreatePixelShader( byteCodePtr, byteCodeSize );
}

RE_HANDLE CDirect3D11::CreateComputeShader( const void* byteCodePtr, std::size_t byteCodeSize )
{
	return m_resourceManager.CreateComputeShader( byteCodePtr, byteCodeSize );
}

RE_HANDLE CDirect3D11::CreateRenderTarget( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	return m_resourceManager.CreateRenderTarget( texHandle, trait );
}

RE_HANDLE CDirect3D11::CreateDepthStencil( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	return m_resourceManager.CreateDepthStencil( texHandle, trait );
}

RE_HANDLE CDirect3D11::CreateTexture1DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	return m_resourceManager.CreateTexture1DShaderResource( texHandle, trait );
}

RE_HANDLE CDirect3D11::CreateTexture2DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	return m_resourceManager.CreateTexture2DShaderResource( texHandle, trait );
}

RE_HANDLE CDirect3D11::CreateTexture3DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	return m_resourceManager.CreateTexture3DShaderResource( texHandle, trait );
}

RE_HANDLE CDirect3D11::CreateBufferShaderResource( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait )
{
	return m_resourceManager.CreateBufferShaderResource( bufHandle, trait );
}

RE_HANDLE CDirect3D11::CreateTexture1DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	return m_resourceManager.CreateTexture1DRandomAccess( texHandle, trait );
}

RE_HANDLE CDirect3D11::CreateTexture2DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait )
{
	return m_resourceManager.CreateTexture2DRandomAccess( texHandle, trait );
}

RE_HANDLE CDirect3D11::CreateBufferRandomAccess( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait )
{
	return m_resourceManager.CreateBufferRandomAccess( bufHandle, trait );
}

void* CDirect3D11::LockBuffer( RE_HANDLE buffer, int lockFlag, UINT subResource )
{
	CD3D11Buffer* d3d11buffer = m_resourceManager.GetBuffer( buffer );
	D3D11_MAPPED_SUBRESOURCE resource;

	HRESULT hr = g_pd3d11DeviceContext->Map( d3d11buffer->Get( ), subResource, ConvertLockFlagToD3D11Map( lockFlag ), 0, &resource );
	if ( FAILED( hr ) )
	{
		__debugbreak();
	}

	return resource.pData;
}

void CDirect3D11::UnLockBuffer( RE_HANDLE buffer, UINT subResource )
{
	CD3D11Buffer* d3d11buffer = m_resourceManager.GetBuffer( buffer );

	g_pd3d11DeviceContext->Unmap( d3d11buffer->Get( ), subResource );
}

void CDirect3D11::EnumerateSampleCountAndQuality( int* size, DXGI_SAMPLE_DESC* pSamples )
{
	assert( size != nullptr );

	constexpr int desireCounts[] = { 2, 4, 8 };
	UINT qualityLevel = 0;
	for ( int i = 0; i < _countof( desireCounts ); ++i )
	{
		HRESULT hr = g_pd3d11Device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, desireCounts[i], &qualityLevel );
		if ( SUCCEEDED( hr ) && qualityLevel > 0 )
		{
			if ( pSamples != nullptr )
			{
				pSamples[i].Count = desireCounts[i];
				pSamples[i].Quality = qualityLevel - 1;
			}
			
			++(*size);
		}
	}
}

void CDirect3D11::SetViewports( const Viewport* viewPorts, int count )
{
	std::vector<D3D11_VIEWPORT> d3d11Viewports;

	for ( int i = 0; i < count; ++i )
	{
		const Viewport& vp = viewPorts[i];
		D3D11_VIEWPORT newVeiwport = { vp.m_x, vp.m_y, vp.m_width, vp.m_height, vp.m_near, vp.m_far };

		d3d11Viewports.push_back( newVeiwport );
	}

	assert( d3d11Viewports.size( ) <= UINT_MAX );
	g_pd3d11DeviceContext->RSSetViewports( static_cast<UINT>( d3d11Viewports.size( ) ), d3d11Viewports.data( ) );
}

void CDirect3D11::SetScissorRects( const RECT* rects, int size )
{
	g_pd3d11DeviceContext->RSSetScissorRects( size, rects );
}

RE_HANDLE CDirect3D11::CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateRasterizerState( trait );
}

RE_HANDLE CDirect3D11::CreateSamplerState( const SAMPLER_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateSamplerState( trait );
}

RE_HANDLE CDirect3D11::CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateDepthStencilState( trait );
}

RE_HANDLE CDirect3D11::CreateBlendState( const BLEND_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateBlendState( trait );
}

void CDirect3D11::ClearRendertarget( RE_HANDLE renderTarget, const float (&clearColor)[4] )
{
	CD3D11RenderTarget* rtv = m_resourceManager.GetRendertarget( renderTarget );
	g_pd3d11DeviceContext->ClearRenderTargetView( rtv->Get(), clearColor );
}

void CDirect3D11::ClearDepthStencil( RE_HANDLE depthStencil, float depthColor, UINT8 stencilColor )
{
	CD3D11DepthStencil* dsv = m_resourceManager.GetDepthstencil( depthStencil );
	g_pd3d11DeviceContext->ClearDepthStencilView( dsv->Get( ), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
}

void CDirect3D11::BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets )
{
	ID3D11Buffer* pBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
	UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
	UINT offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

	if ( pVertexBuffers )
	{
		assert( pStrides != nullptr );
		assert( pOffsets != nullptr );
		for ( UINT i = 0; i < numBuffers; ++i )
		{
			if ( pVertexBuffers[i].IsValid( ) )
			{
				CD3D11Buffer* d3d11buffer = m_resourceManager.GetBuffer( pVertexBuffers[i] );
				pBuffers[i] = d3d11buffer->Get( );
				strides[i] = pStrides[i];
				offsets[i] = pOffsets[i];
			}
		}
	}

	g_pd3d11DeviceContext->IASetVertexBuffers( startSlot, numBuffers, pBuffers, pStrides, pOffsets );
}

void CDirect3D11::BindIndexBuffer( RE_HANDLE indexBuffer, UINT indexOffset )
{
	ID3D11Buffer* buffer = nullptr;
	DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;

	if ( indexBuffer.IsValid( ) )
	{
		CD3D11Buffer* d3d11buffer = m_resourceManager.GetBuffer( indexBuffer );
		buffer = d3d11buffer->Get( );
		
		if ( d3d11buffer->Stride() == 4 )
		{
			format = DXGI_FORMAT_R32_UINT;
		}
	}

	g_pd3d11DeviceContext->IASetIndexBuffer( buffer, format, indexOffset );
}

void CDirect3D11::BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, const RE_HANDLE* pConstantBuffers )
{
	ID3D11Buffer* pBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( pConstantBuffers[i].IsValid( ) )
		{
			CD3D11Buffer* d3d11buffer = m_resourceManager.GetBuffer( pConstantBuffers[i] );
			pBuffers[i] = d3d11buffer->Get( );
		}
	}

	switch ( type )
	{
	case SHADER_TYPE::VS:
		g_pd3d11DeviceContext->VSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::HS:
		g_pd3d11DeviceContext->HSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::DS:
		g_pd3d11DeviceContext->DSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::GS:
		g_pd3d11DeviceContext->GSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::PS:
		g_pd3d11DeviceContext->PSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::CS:
		g_pd3d11DeviceContext->CSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	default:
		break;
	}
}

void CDirect3D11::BindVertexLayout( RE_HANDLE layout )
{
	assert( IsVertexLayout( layout ) );
	ID3D11InputLayout* inputLayout = nullptr;
	if ( layout.IsValid( ) )
	{
		CD3D11VertexLayout* d3d11Layout = m_resourceManager.GetVertexLayout( layout );
		inputLayout = d3d11Layout->Get( );
	}

	g_pd3d11DeviceContext->IASetInputLayout( inputLayout );
}

void CDirect3D11::BindShader( RE_HANDLE shader )
{
	if ( IsVertexShaderHandle( shader ) )
	{
		ID3D11VertexShader* vs = nullptr;
		if ( shader.IsValid( ) )
		{
			CD3D11VertexShader* d3d11VS = m_resourceManager.GetVertexShader( shader );
			vs = d3d11VS->Get( );
		}

		g_pd3d11DeviceContext->VSSetShader( vs, nullptr, 0 );
	}
	else if ( IsGeometryShaderHandle( shader ) )
	{
		ID3D11GeometryShader* gs = nullptr;
		if ( shader.IsValid( ) )
		{
			CD3D11GeometryShader* d3d11GS = m_resourceManager.GetGeometryShader( shader );
			gs = d3d11GS->Get( );
		}

		g_pd3d11DeviceContext->GSSetShader( gs, nullptr, 0 );
	}
	else if ( IsPixelShaderHandle( shader ) )
	{
		ID3D11PixelShader* ps = nullptr;
		if ( shader.IsValid( ) )
		{
			CD3D11PixelShader* d3d11PS = m_resourceManager.GetPixelShader( shader );
			ps = d3d11PS->Get( );
		}

		g_pd3d11DeviceContext->PSSetShader( ps, nullptr, 0 );
	}
	else if ( IsComputeShaderHandle( shader ) )
	{
		ID3D11ComputeShader* cs = nullptr;
		if ( shader.IsValid( ) )
		{
			CD3D11ComputeShader* d3d11CS = m_resourceManager.GetComputeShader( shader );
			cs = d3d11CS->Get( );
		}

		g_pd3d11DeviceContext->CSSetShader( cs, nullptr, 0 );
	}
	else
	{
		__debugbreak( );
	}
}

void CDirect3D11::BindShaderResource( SHADER_TYPE type, int startSlot, int count, const RE_HANDLE* resource )
{
	ID3D11ShaderResourceView* pSrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

	if ( resource )
	{
		for ( int i = 0; i < count; ++i )
		{
			if ( resource[i].IsValid( ) )
			{
				CD3D11ShaderResource* d3d11SRV = m_resourceManager.GetShaderResource( resource[i] );
				pSrvs[i] = d3d11SRV->Get( );
			}
		}
	}

	switch ( type )
	{
	case SHADER_TYPE::VS:
		g_pd3d11DeviceContext->VSSetShaderResources( startSlot, count, pSrvs );
		break;
	case SHADER_TYPE::HS:
		g_pd3d11DeviceContext->HSSetShaderResources( startSlot, count, pSrvs );
		break;
	case SHADER_TYPE::DS:
		g_pd3d11DeviceContext->DSSetShaderResources( startSlot, count, pSrvs );
		break;
	case SHADER_TYPE::GS:
		g_pd3d11DeviceContext->GSSetShaderResources( startSlot, count, pSrvs );
		break;
	case SHADER_TYPE::PS:
		g_pd3d11DeviceContext->PSSetShaderResources( startSlot, count, pSrvs );
		break;
	case SHADER_TYPE::CS:
		g_pd3d11DeviceContext->CSSetShaderResources( startSlot, count, pSrvs );
		break;
	default:
		__debugbreak( );
		break;
	}
}

void CDirect3D11::BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource )
{
	ID3D11UnorderedAccessView* pRavs[64] = {};
	UINT initialCounts[64] = {};

	if ( resource )
	{
		for ( int i = 0; i < count; ++i )
		{
			if ( resource[i].IsValid( ) )
			{
				CD3D11RandomAccessResource* d3d11RAV = m_resourceManager.GetRandomAccess( resource[i] );
				pRavs[i] = d3d11RAV->Get( );
			}
		}
	}

	g_pd3d11DeviceContext->CSSetUnorderedAccessViews( startSlot, count, pRavs, initialCounts );
}

void CDirect3D11::BindRenderTargets( const RE_HANDLE* pRenderTargets, int renderTargetCount, RE_HANDLE depthStencil )
{
	ID3D11RenderTargetView* pRtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

	for ( int i = 0; i < renderTargetCount; ++i )
	{
		if ( pRenderTargets[i].IsValid( ) )
		{
			pRtvs[i] = m_resourceManager.GetRendertarget( pRenderTargets[i] )->Get( );
		}
		else
		{
			pRtvs[i] = nullptr;
		}
	}
	
	ID3D11DepthStencilView* pDsv = nullptr;
	if ( depthStencil.IsValid( ) )
	{
		pDsv = m_resourceManager.GetDepthstencil( depthStencil )->Get();
	}

	g_pd3d11DeviceContext->OMSetRenderTargets( static_cast<UINT>( renderTargetCount ), pRtvs, pDsv );
}

void CDirect3D11::BindRasterizerState( RE_HANDLE rasterizerState )
{
	ID3D11RasterizerState* pState = nullptr;

	if ( rasterizerState.IsValid( ) )
	{
		pState = m_resourceManager.GetRasterizerState( rasterizerState )->Get();
	}

	g_pd3d11DeviceContext->RSSetState( pState );
}

void CDirect3D11::BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, const RE_HANDLE* pSamplerStates )
{
	ID3D11SamplerState* pStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};

	if ( pSamplerStates )
	{
		for ( int i = 0; i < numSamplers; ++i )
		{
			if ( pSamplerStates[i].IsValid( ) )
			{
				pStates[i] = m_resourceManager.GetSamplerState( pSamplerStates[i] )->Get( );
			}
		}
	}
	
	switch ( type )
	{
	case SHADER_TYPE::VS:
		{
			g_pd3d11DeviceContext->VSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::HS:
		{
			g_pd3d11DeviceContext->HSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::DS:
		{
			g_pd3d11DeviceContext->DSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::GS:
		{
			g_pd3d11DeviceContext->GSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::PS:
		{
			g_pd3d11DeviceContext->PSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::CS:
		{
			g_pd3d11DeviceContext->CSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	default:
		__debugbreak( );
		break;
	}
}

void CDirect3D11::BindDepthStencilState( RE_HANDLE depthStencilState )
{
	if ( depthStencilState.IsValid( ) )
	{
		CD3D11DepthStencilState* state = m_resourceManager.GetDepthStencilState( depthStencilState );
		g_pd3d11DeviceContext->OMSetDepthStencilState( state->Get( ), state->GetStencilRef( ) );
	}
	else
	{
		g_pd3d11DeviceContext->OMSetDepthStencilState( nullptr, 0 );
	}
}

void CDirect3D11::BindBlendState( RE_HANDLE blendState )
{
	if ( blendState.IsValid( ) )
	{
		CD3D11BlendState* state = m_resourceManager.GetBlendState( blendState );
		g_pd3d11DeviceContext->OMSetBlendState( state->Get( ), state->GetBlendFactor( ), state->GetSamplerMask( ) );
	}
	else
	{
		float defaultBlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		g_pd3d11DeviceContext->OMSetBlendState( nullptr, defaultBlendFactor, D3D11_DEFAULT_SAMPLE_MASK );
	}
}

void CDirect3D11::Draw( UINT primitive, UINT vertexCount, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	g_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	g_pd3d11DeviceContext->Draw( vertexCount, vertexOffset );
}

void CDirect3D11::DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	g_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	g_pd3d11DeviceContext->DrawIndexed( indexCount, indexOffset, vertexOffset );
}

void CDirect3D11::DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	g_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	g_pd3d11DeviceContext->DrawInstanced( vertexCount, instanceCount, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	g_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	g_pd3d11DeviceContext->DrawIndexedInstanced( indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawAuto( )
{
	g_pd3d11DeviceContext->DrawAuto( );
}

void CDirect3D11::Dispatch( int x, int y, int z )
{
	g_pd3d11DeviceContext->Dispatch( static_cast<UINT>( x ), static_cast<UINT>( y ), static_cast<UINT>( z ) );
}

BYTE CDirect3D11::Present( )
{
	HRESULT hr = m_pdxgiSwapChain->Present( 0, 0 );

	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{
		return DEVICE_ERROR::DEVICE_LOST;
	}

	return DEVICE_ERROR::NONE;
}

void CDirect3D11::GenerateMips( RE_HANDLE shaderResource )
{
	CD3D11ShaderResource* srv = m_resourceManager.GetShaderResource( shaderResource );
	g_pd3d11DeviceContext->GenerateMips( srv->Get( ) );
}

void CDirect3D11::GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option )
{
	assert( option != nullptr );
	option->m_count = m_multiSampleOption.Count;
	option->m_quality = m_multiSampleOption.Quality;
}

CDirect3D11::CDirect3D11( )
{
	// RegisterGraphicsEnumString( );
}

CDirect3D11::~CDirect3D11( )
{
	Shutdown( );
}

bool CDirect3D11::CreateDeviceDependentResource( HWND hWnd, UINT nWndWidth, UINT nWndHeight )
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

	UINT flag = 0;
#ifdef _DEBUG
	flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL selectedFeature = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = E_FAIL;

	for ( int i = 0; i < _countof( d3dDriverTypes ); ++i )
	{
		hr = D3D11CreateDevice( nullptr,
			d3dDriverTypes[i],
			nullptr, flag,
			d3dFeatureLevel,
			_countof( d3dFeatureLevel ),
			D3D11_SDK_VERSION,
			g_pd3d11Device.GetAddressOf( ),
			&selectedFeature,
			g_pd3d11DeviceContext.GetAddressOf( )
		);

		if ( SUCCEEDED( hr ) )
		{
			int desiredSampleCount = 0;
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

			hr = m_pdxgiFactory->CreateSwapChain( g_pd3d11Device.Get( ), &dxgiSwapchainDesc, m_pdxgiSwapChain.GetAddressOf( ) );
			if ( FAILED( hr ) )
			{
				return false;
			}

			m_resourceManager.OnDeviceRestore( g_pd3d11Device.Get( ), g_pd3d11DeviceContext.Get( ) );

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
	if ( g_pd3d11Device == nullptr )
	{
		return;
	}

	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Debug> pD3dDebug;

	hr = g_pd3d11Device.Get( )->QueryInterface( IID_PPV_ARGS( &pD3dDebug ) );

	if ( SUCCEEDED( hr ) )
	{
		pD3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL );
	}
}

void CreateAbstractGraphicsApi( )
{
	g_AbstractGraphicsApi = new CDirect3D11( );
}

void DestoryAbstractGraphicsApi( )
{
	delete g_AbstractGraphicsApi;
	g_pd3d11DeviceContext.Reset( );
	g_pd3d11Device.Reset( );
}

ID3D11Device& D3D11Device( )
{
	return *g_pd3d11Device.Get( );
}

ID3D11DeviceContext& D3D11Context( )
{
	return *g_pd3d11DeviceContext.Get();
}

void* GetD3D11GraphicsApi( )
{
	return g_AbstractGraphicsApi;
}
