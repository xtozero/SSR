#include "stdafx.h"
#include "D3D11Api.h"

#include "common.h"
#include "Common/IAga.h"
#include "D3D11ResourceInterface.h"

#include "D3D11BaseTexture.h"
#include "D3D11BlendState.h"
#include "D3D11Buffer.h"
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
#include "D3D11Viewport.h"

#include "DataStructure/EnumStringMap.h"

#include "ShaderPrameterMap.h"

#include "Texture.h"

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
};

class CDirect3D11 : public IAga
{
public:
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;
	virtual void WaitGPU( ) override;

	virtual aga::Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	virtual aga::Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) override;

	virtual VertexLayout* FindAndCreateVertexLayout( aga::VertexShader* vs, const VertexLayoutDesc& layoutDesc ) override;
	virtual RE_HANDLE CreateGeometryShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual aga::ComputeShader* CreateComputeShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual aga::VertexShader* CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual aga::PixelShader* CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) override;
	virtual const ShaderParameterInfo& GetShaderParameterInfo( RE_HANDLE shader ) const override;

	virtual RE_HANDLE CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) override;
	virtual aga::DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) override;
	virtual RE_HANDLE CreateBlendState( const BLEND_STATE_TRAIT& trait ) override;

	virtual aga::Viewport* CreateViewport( int width, int height, void* hWnd, RESOURCE_FORMAT format ) override;

	virtual void* Lock( aga::Buffer* buffer, int lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) override;
	virtual void UnLock( aga::Buffer* buffer, UINT subResource = 0 ) override;

	virtual void SetViewports( const aga::Viewport* viewPorts, int count ) override;
	virtual void SetViewport( UINT minX, UINT minY, float minZ, UINT maxX, UINT maxY, float maxZ ) override;
	virtual void SetScissorRects( const aga::Viewport* viewPorts, int size ) override;
	virtual void SetScissorRect( UINT minX, UINT minY, UINT maxX, UINT maxY ) override;

	virtual void ClearDepthStencil( aga::Texture* depthStencil, float depthColor, UINT8 stencilColor ) override;

	virtual void BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets ) override;
	virtual void BindIndexBuffer( RE_HANDLE indexBuffer, UINT indexOffset ) override;
	virtual void BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, aga::Buffer** pConstantBuffers ) override;
	virtual void BindVertexLayout( RE_HANDLE layout ) override;
	virtual void BindShader( RE_HANDLE shader ) override;
	virtual void BindShader( aga::ComputeShader* shader ) override;

	virtual void BindConstant( aga::VertexShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) override;
	virtual void BindShaderInput( aga::VertexShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) override;

	virtual void BindConstant( aga::PixelShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) override;
	virtual void BindShaderInput( aga::PixelShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) override;

	virtual void BindConstant( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) override;
	virtual void BindShaderInput( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) override;
	virtual void BindShaderOutput( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) override;

	virtual void BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource ) override;
	virtual void BindRenderTargets( aga::Texture** pRenderTargets, int renderTargetCount, aga::Texture* depthStencil ) override;
	virtual void BindRasterizerState( RE_HANDLE rasterizerState ) override;
	virtual void BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, const RE_HANDLE* pSamplerStates ) override;
	//virtual void BindDepthStencilState( RE_HANDLE depthStencilState ) override;
	virtual void BindBlendState( RE_HANDLE blendState ) override;

	virtual void Draw( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT vertexOffset = 0 ) override;
	virtual void DrawIndexed( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) override;
	virtual void DrawInstanced( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawInstancedInstanced( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawAuto( ) override;
	virtual void Dispatch( UINT x, UINT y, UINT z = 1 ) override;

	virtual void Copy( aga::Buffer* dst, aga::Buffer* src, std::size_t size ) override;

	virtual void GenerateMips( RE_HANDLE shaderResource ) override;

	virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) override;

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

	bool CreateDeviceDependentResource( HWND hWnd, UINT nWndWidth, UINT nWndHeight );
	bool CreateDeviceIndependentResource( );
	void ReportLiveDevice( );
	void EnumerateSampleCountAndQuality( int* size, DXGI_SAMPLE_DESC* pSamples );

	Microsoft::WRL::ComPtr<IDXGIFactory1>					m_pdxgiFactory;

	Microsoft::WRL::ComPtr<ID3D11Device>					m_pd3d11Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pd3d11DeviceContext;

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

	if ( !m_resourceManager.Bootup( m_pd3d11Device.Get( ), m_pd3d11DeviceContext.Get( ) ) )
	{
		return false;
	}

	return true;
}

void CDirect3D11::HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight )
{
	m_resourceManager.OnDeviceLost( );

	m_pdxgiFactory.Reset( );
	m_pd3d11DeviceContext.Reset( );
	m_pd3d11Device.Reset( );

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
	if ( m_pd3d11Device == nullptr )
	{
		__debugbreak( );
	}

	// TODO: viewport ���� ������ ũ�� ��ȯ ���� ó���� �� �� �ֵ��� �߰�
	//if ( m_pdxgiSwapChain == nullptr )
	//{
	//	__debugbreak( );
	//}

	m_resourceManager.AppSizeChanged( nWndWidth, nWndHeight );

	//HRESULT hr = m_pdxgiSwapChain->ResizeBuffers( 1, nWndWidth, nWndHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
	//if ( FAILED( hr ) )
	//{
	//	__debugbreak( );
	//}
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
		while( m_pd3d11DeviceContext->GetData( pQuery.Get( ), &data, sizeof( data ), 0 ) != S_OK ) { }
	}
}

aga::Texture* CDirect3D11::CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	return m_resourceManager.CreateTexture( trait, initData );
}

aga::Buffer* CDirect3D11::CreateBuffer( const BUFFER_TRAIT& trait, const void* initData )
{
	return m_resourceManager.CreateBuffer( trait, initData );
}

VertexLayout* CDirect3D11::FindAndCreateVertexLayout( aga::VertexShader* vs, const VertexLayoutDesc& layoutDesc )
{
	return m_resourceManager.FindAndCreateVertexLayout( vs, layoutDesc );
}

RE_HANDLE CDirect3D11::CreateGeometryShader( const void* byteCode, std::size_t byteCodeSize )
{
	// TODO : Need to be implemented later
	//return m_resourceManager.CreateGeometryShader( byteCode, byteCodeSize );
	return {};
}

aga::ComputeShader* CDirect3D11::CreateComputeShader( const void* byteCode, std::size_t byteCodeSize )
{
	return m_resourceManager.CreateComputeShader( byteCode, byteCodeSize );
}

aga::VertexShader* CDirect3D11::CreateVertexShader( const void* byteCode, std::size_t byteCodeSize )
{
	return m_resourceManager.CreateVertexShader( byteCode, byteCodeSize );
}

aga::PixelShader* CDirect3D11::CreatePixelShader( const void* byteCode, std::size_t byteCodeSize )
{
	return m_resourceManager.CreatePixelShader( byteCode, byteCodeSize );
}

const ShaderParameterInfo& CDirect3D11::GetShaderParameterInfo( RE_HANDLE shader ) const
{
	// TODO : Need to be implemented later
	//assert( shader.IsValid( ) );
	//if ( IsVertexShaderHandle( shader ) )
	//{
	//	CD3D11VertexShader* d3d11VS = m_resourceManager.GetVertexShader( shader );
	//	return d3d11VS->GetParameterInfo( );
	//}
	//else if ( IsGeometryShaderHandle( shader ) )
	//{
	//	CD3D11GeometryShader* d3d11GS = m_resourceManager.GetGeometryShader( shader );
	//	return d3d11GS->GetParameterInfo( );
	//}
	//else if ( IsPixelShaderHandle( shader ) )
	//{
	//	CD3D11PixelShader* d3d11PS = m_resourceManager.GetPixelShader( shader );
	//	return d3d11PS->GetParameterInfo( );
	//}
	//else
	//{
	//	assert( IsComputeShaderHandle( shader ) );
	//	CD3D11ComputeShader* d3d11CS = m_resourceManager.GetComputeShader( shader );
	//	return d3d11CS->GetParameterInfo( );
	//}
	return {};
}

void* CDirect3D11::Lock( aga::Buffer* buffer, int lockFlag, UINT subResource )
{
	if ( buffer == nullptr )
	{
		return nullptr;
	}

	D3D11BufferBase* d3d11buffer = static_cast<D3D11BufferBase*>( buffer );
	D3D11_MAPPED_SUBRESOURCE resource;

	HRESULT hr = m_pd3d11DeviceContext->Map( d3d11buffer->Buffer( ), subResource, ConvertLockFlagToD3D11Map( lockFlag ), 0, &resource );
	if ( FAILED( hr ) )
	{
		__debugbreak();
	}

	return resource.pData;
}

void CDirect3D11::UnLock( aga::Buffer* buffer, UINT subResource )
{
	if ( buffer == nullptr )
	{
		return;
	}

	D3D11BufferBase* d3d11buffer = static_cast<D3D11BufferBase*>( buffer );

	m_pd3d11DeviceContext->Unmap( d3d11buffer->Buffer( ), subResource );
}

void CDirect3D11::EnumerateSampleCountAndQuality( int* size, DXGI_SAMPLE_DESC* pSamples )
{
	assert( size != nullptr );

	constexpr int desireCounts[] = { 2, 4, 8 };
	UINT qualityLevel = 0;
	for ( int i = 0; i < _countof( desireCounts ); ++i )
	{
		HRESULT hr = m_pd3d11Device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, desireCounts[i], &qualityLevel );
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

void CDirect3D11::SetViewports( const aga::Viewport* viewPorts, int size )
{
	std::vector<D3D11_VIEWPORT> d3d11Viewports;

	for ( int i = 0; i < size; ++i )
	{
		const auto& vp = static_cast<const aga::D3D11Viewport*>( viewPorts )[i];
		auto renderTargetSize = vp.Size( );
		D3D11_VIEWPORT newVeiwport = { 
			0.f, 
			0.f, 
			static_cast<float>( renderTargetSize.first ),
			static_cast<float>( renderTargetSize.second ),
			0.f,
			1.f };

		d3d11Viewports.push_back( newVeiwport );
	}

	assert( d3d11Viewports.size( ) <= UINT_MAX );
	m_pd3d11DeviceContext->RSSetViewports( static_cast<UINT>( d3d11Viewports.size( ) ), d3d11Viewports.data( ) );
}

void CDirect3D11::SetViewport( UINT minX, UINT minY, float minZ, UINT maxX, UINT maxY, float maxZ )
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

void CDirect3D11::SetScissorRects( const aga::Viewport* viewPorts, int size )
{
	std::vector<D3D11_RECT> d3d11Rects;

	for ( int i = 0; i < size; ++i )
	{
		const auto& vp = static_cast<const aga::D3D11Viewport*>( viewPorts )[i];
		auto renderTargetSize = vp.Size( );
		D3D11_RECT newVeiwport = { 
			0L, 
			0L, 
			static_cast<LONG>( renderTargetSize.first ),
			static_cast<LONG>( renderTargetSize.second ) };

		d3d11Rects.push_back( newVeiwport );
	}

	m_pd3d11DeviceContext->RSSetScissorRects( static_cast<UINT>( d3d11Rects.size( ) ), d3d11Rects.data( ) );
}

void CDirect3D11::SetScissorRect( UINT minX, UINT minY, UINT maxX, UINT maxY )
{
	D3D11_RECT rect = {};
	rect.left = minX;
	rect.top = minY;
	rect.right = maxX;
	rect.bottom = maxY;

	m_pd3d11DeviceContext->RSSetScissorRects( 1, &rect );
}

RE_HANDLE CDirect3D11::CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateRasterizerState( trait );
}

RE_HANDLE CDirect3D11::CreateSamplerState( const SAMPLER_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateSamplerState( trait );
}

aga::DepthStencilState* CDirect3D11::CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateDepthStencilState( trait );
}

RE_HANDLE CDirect3D11::CreateBlendState( const BLEND_STATE_TRAIT& trait )
{
	return m_resourceManager.CreateBlendState( trait );
}

aga::Viewport* CDirect3D11::CreateViewport( int width, int height, void* hWnd, RESOURCE_FORMAT format )
{
	return m_resourceManager.CreateViewport( width, height, hWnd, ConvertFormatToDxgiFormat( format ) );
}

void CDirect3D11::ClearDepthStencil( aga::Texture* depthStencil, float depthColor, UINT8 stencilColor )
{
	if ( depthStencil == nullptr )
	{
		return;
	}

	auto baseTexture = reinterpret_cast<aga::D3D11BaseTexture*>( depthStencil );
	ID3D11DepthStencilView* dsv = baseTexture->DepthStencilView( );

	m_pd3d11DeviceContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
}

void CDirect3D11::BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets )
{
	// TODO : Need to be implemented later
	//ID3D11Buffer* pBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
	//UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
	//UINT offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

	//if ( pVertexBuffers )
	//{
	//	assert( pStrides != nullptr );
	//	assert( pOffsets != nullptr );
	//	for ( UINT i = 0; i < numBuffers; ++i )
	//	{
	//		if ( pVertexBuffers[i].IsValid( ) )
	//		{
	//			CD3D11Buffer* d3d11buffer = m_resourceManager.GetBuffer( pVertexBuffers[i] );
	//			pBuffers[i] = d3d11buffer->Get( );
	//			strides[i] = pStrides[i];
	//			offsets[i] = pOffsets[i];
	//		}
	//	}
	//}

	//m_pd3d11DeviceContext->IASetVertexBuffers( startSlot, numBuffers, pBuffers, pStrides, pOffsets );
}

void CDirect3D11::BindIndexBuffer( RE_HANDLE indexBuffer, UINT indexOffset )
{
	// TODO : Need to be implemented later
	//ID3D11Buffer* buffer = nullptr;
	//DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;

	//if ( indexBuffer.IsValid( ) )
	//{
	//	CD3D11Buffer* d3d11buffer = m_resourceManager.GetBuffer( indexBuffer );
	//	buffer = d3d11buffer->Get( );
	//	
	//	if ( d3d11buffer->Stride() == 4 )
	//	{
	//		format = DXGI_FORMAT_R32_UINT;
	//	}
	//}

	//m_pd3d11DeviceContext->IASetIndexBuffer( buffer, format, indexOffset );
}

void CDirect3D11::BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, aga::Buffer** pConstantBuffers )
{
	ID3D11Buffer* pBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pConstantBuffers[i] ) )
		{
			pBuffers[i] = d3d11buffer->Buffer( );
		}
	}

	switch ( type )
	{
	case SHADER_TYPE::VS:
		m_pd3d11DeviceContext->VSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::HS:
		m_pd3d11DeviceContext->HSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::DS:
		m_pd3d11DeviceContext->DSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::GS:
		m_pd3d11DeviceContext->GSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::PS:
		m_pd3d11DeviceContext->PSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	case SHADER_TYPE::CS:
		m_pd3d11DeviceContext->CSSetConstantBuffers( startSlot, numBuffers, pBuffers );
		break;
	default:
		break;
	}
}

void CDirect3D11::BindVertexLayout( RE_HANDLE layout )
{
	// TODO : Need to be implemented later

	//assert( IsVertexLayout( layout ) );
	//ID3D11InputLayout* inputLayout = nullptr;
	//if ( layout.IsValid( ) )
	//{
	//	CD3D11VertexLayout* d3d11Layout = m_resourceManager.GetVertexLayout( layout );
	//	inputLayout = d3d11Layout->Get( );
	//}

	//m_pd3d11DeviceContext->IASetInputLayout( inputLayout );
}

void CDirect3D11::BindShader( RE_HANDLE shader )
{
	// TODO : Need to be implemented later

	//if ( IsVertexShaderHandle( shader ) )
	//{
	//	ID3D11VertexShader* vs = nullptr;
	//	if ( shader.IsValid( ) )
	//	{
	//		CD3D11VertexShader* d3d11VS = m_resourceManager.GetVertexShader( shader );
	//		vs = d3d11VS->Get( );
	//	}

	//	m_pd3d11DeviceContext->VSSetShader( vs, nullptr, 0 );
	//}
	//else if ( IsGeometryShaderHandle( shader ) )
	//{
	//	ID3D11GeometryShader* gs = nullptr;
	//	if ( shader.IsValid( ) )
	//	{
	//		CD3D11GeometryShader* d3d11GS = m_resourceManager.GetGeometryShader( shader );
	//		gs = d3d11GS->Get( );
	//	}

	//	m_pd3d11DeviceContext->GSSetShader( gs, nullptr, 0 );
	//}
	//else if ( IsPixelShaderHandle( shader ) )
	//{
	//	ID3D11PixelShader* ps = nullptr;
	//	if ( shader.IsValid( ) )
	//	{
	//		CD3D11PixelShader* d3d11PS = m_resourceManager.GetPixelShader( shader );
	//		ps = d3d11PS->Get( );
	//	}

	//	m_pd3d11DeviceContext->PSSetShader( ps, nullptr, 0 );
	//}
	//else if ( IsComputeShaderHandle( shader ) )
	//{
	//	ID3D11ComputeShader* cs = nullptr;
	//	if ( shader.IsValid( ) )
	//	{
	//		CD3D11ComputeShader* d3d11CS = m_resourceManager.GetComputeShader( shader );
	//		cs = d3d11CS->Get( );
	//	}

	//	m_pd3d11DeviceContext->CSSetShader( cs, nullptr, 0 );
	//}
	//else
	//{
	//	__debugbreak( );
	//}
}

void CDirect3D11::BindShader( aga::ComputeShader* shader )
{
	ID3D11ComputeShader* cs = nullptr;
	if ( D3D11ComputeShader* d3d11CS = static_cast<D3D11ComputeShader*>( shader ) )
	{
		cs = d3d11CS->Resource( );
	}

	D3D11Context( ).CSSetShader( cs, nullptr, 0 );
}

void CDirect3D11::BindConstant( aga::VertexShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers )
{
	ID3D11Buffer* pConstants[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pBuffers[i] ) )
		{
			pConstants[i] = d3d11buffer->Buffer( );
		}
	}

	m_pd3d11DeviceContext->VSSetConstantBuffers( startSlot, numBuffers, pConstants );
}

void CDirect3D11::BindShaderInput( aga::VertexShader* shader, int startSlot, int numBuffers, aga::Buffer ** pBuffers )
{
	ID3D11ShaderResourceView* pSrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pBuffers[i] ) )
		{
			pSrvs[i] = d3d11buffer->Srv( );
		}
	}

	m_pd3d11DeviceContext->VSSetShaderResources( startSlot, numBuffers, pSrvs );
}

void CDirect3D11::BindConstant( aga::PixelShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers )
{
	ID3D11Buffer* pConstants[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pBuffers[i] ) )
		{
			pConstants[i] = d3d11buffer->Buffer( );
		}
	}

	m_pd3d11DeviceContext->CSSetConstantBuffers( startSlot, numBuffers, pConstants );
}

void CDirect3D11::BindShaderInput( aga::PixelShader* shader, int startSlot, int numBuffers, aga::Buffer ** pBuffers )
{
	ID3D11ShaderResourceView* pSrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pBuffers[i] ) )
		{
			pSrvs[i] = d3d11buffer->Srv( );
		}
	}

	m_pd3d11DeviceContext->CSSetShaderResources( startSlot, numBuffers, pSrvs );
}

void CDirect3D11::BindConstant( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers )
{
	ID3D11Buffer* pConstants[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pBuffers[i] ) )
		{
			pConstants[i] = d3d11buffer->Buffer( );
		}
	}

	m_pd3d11DeviceContext->CSSetConstantBuffers( startSlot, numBuffers, pConstants );
}

void CDirect3D11::BindShaderInput( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer ** pBuffers )
{
	ID3D11ShaderResourceView* pSrvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pBuffers[i] ) )
		{
			pSrvs[i] = d3d11buffer->Srv( );
		}
	}

	m_pd3d11DeviceContext->CSSetShaderResources( startSlot, numBuffers, pSrvs );
}

void CDirect3D11::BindShaderOutput( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers )
{
	ID3D11UnorderedAccessView* pUavs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( auto d3d11buffer = static_cast<D3D11BufferBase*>( pBuffers[i] ) )
		{
			pUavs[i] = d3d11buffer->Uav( );
		}
	}

	m_pd3d11DeviceContext->CSSetUnorderedAccessViews( startSlot, numBuffers, pUavs, nullptr );
}

void CDirect3D11::BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource )
{
	// TODO : Need to be implemented later

	/*ID3D11UnorderedAccessView* pRavs[64] = {};
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

	m_pd3d11DeviceContext->CSSetUnorderedAccessViews( startSlot, count, pRavs, initialCounts );*/
}

void CDirect3D11::BindRenderTargets( aga::Texture** pRenderTargets, int renderTargetCount, aga::Texture* depthStencil )
{
	ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

	for ( int i = 0; i < renderTargetCount; ++i )
	{
		auto rtTex = static_cast<aga::D3D11BaseTexture*>( pRenderTargets[i] );
		if ( rtTex )
		{
			rtvs[i] = rtTex->RenderTargetView( );
		}
	}
	
	ID3D11DepthStencilView* dsv = nullptr;

	auto dsTex = static_cast<aga::D3D11BaseTexture*>( depthStencil );
	if ( dsTex )
	{
		dsv = dsTex->DepthStencilView( );
	}

	m_pd3d11DeviceContext->OMSetRenderTargets( static_cast<UINT>( renderTargetCount ), rtvs, dsv );
}

void CDirect3D11::BindRasterizerState( RE_HANDLE rasterizerState )
{
	ID3D11RasterizerState* pState = nullptr;

	if ( rasterizerState.IsValid( ) )
	{
		pState = m_resourceManager.GetRasterizerState( rasterizerState )->Get();
	}

	m_pd3d11DeviceContext->RSSetState( pState );
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
			m_pd3d11DeviceContext->VSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::HS:
		{
			m_pd3d11DeviceContext->HSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::DS:
		{
			m_pd3d11DeviceContext->DSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::GS:
		{
			m_pd3d11DeviceContext->GSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::PS:
		{
			m_pd3d11DeviceContext->PSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	case SHADER_TYPE::CS:
		{
			m_pd3d11DeviceContext->CSSetSamplers( startSlot, numSamplers, pStates );
		}
		break;
	default:
		__debugbreak( );
		break;
	}
}

//void CDirect3D11::BindDepthStencilState( RE_HANDLE depthStencilState )
//{
//	if ( depthStencilState.IsValid( ) )
//	{
//		D3D11DepthStencilState* state = m_resourceManager.GetDepthStencilState( depthStencilState );
//		m_pd3d11DeviceContext->OMSetDepthStencilState( state->Get( ), state->GetStencilRef( ) );
//	}
//	else
//	{
//		m_pd3d11DeviceContext->OMSetDepthStencilState( nullptr, 0 );
//	}
//}

void CDirect3D11::BindBlendState( RE_HANDLE blendState )
{
	if ( blendState.IsValid( ) )
	{
		CD3D11BlendState* state = m_resourceManager.GetBlendState( blendState );
		m_pd3d11DeviceContext->OMSetBlendState( state->Get( ), state->GetBlendFactor( ), state->GetSamplerMask( ) );
	}
	else
	{
		float defaultBlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		m_pd3d11DeviceContext->OMSetBlendState( nullptr, defaultBlendFactor, D3D11_DEFAULT_SAMPLE_MASK );
	}
}

void CDirect3D11::Draw( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->Draw( vertexCount, vertexOffset );
}

void CDirect3D11::DrawIndexed( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT indexOffset, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawIndexed( indexCount, indexOffset, vertexOffset );
}

void CDirect3D11::DrawInstanced( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawInstanced( vertexCount, instanceCount, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawInstancedInstanced( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT instanceCount, UINT indexOffset, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pd3d11DeviceContext->IASetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawIndexedInstanced( indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawAuto( )
{
	m_pd3d11DeviceContext->DrawAuto( );
}

void CDirect3D11::Dispatch( UINT x, UINT y, UINT z )
{
	m_pd3d11DeviceContext->Dispatch( x, y, z );
}

void CDirect3D11::Copy( aga::Buffer* dst, aga::Buffer* src, std::size_t size )
{
	D3D11BufferBase* d3d11Dst = static_cast<D3D11BufferBase*>( dst );
	D3D11BufferBase* d3d11Src = static_cast<D3D11BufferBase*>( src );

	if ( ( d3d11Dst == nullptr ) || ( d3d11Src == nullptr ) )
	{
		return;
	}

	ID3D11Buffer* dstBuffer = d3d11Dst->Buffer( );
	ID3D11Buffer* srcBuffer = d3d11Src->Buffer( );

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

void CDirect3D11::GenerateMips( RE_HANDLE shaderResource )
{
	// TODO : Need to be implemented later

	//CD3D11ShaderResource* srv = m_resourceManager.GetShaderResource( shaderResource );
	//m_pd3d11DeviceContext->GenerateMips( srv->Get( ) );
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
			m_pd3d11Device.GetAddressOf( ),
			&selectedFeature,
			m_pd3d11DeviceContext.GetAddressOf( )
		);

		if ( SUCCEEDED( hr ) )
		{
			// TODO: Viewport ���� ��Ƽ ���ø� ���� ��� ����
			/*int desiredSampleCount = 0;
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

void CreateAbstractGraphicsApi( )
{
	g_AbstractGraphicsApi = new CDirect3D11( );
}

void DestoryAbstractGraphicsApi( )
{
	delete g_AbstractGraphicsApi;
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

void* GetD3D11GraphicsApi( )
{
	return g_AbstractGraphicsApi;
}
