#include "stdafx.h"

#include "common.h"
#include "Core/RenderCoreDllFunc.h"
#include "D3D11/D3D11RenderStateManager.h"
#include "D3D11/D3D11Resource.h"
#include "D3D11/D3D11ResourceManager.h"
#include "DataStructure/EnumStringMap.h"
#include "Render/IRenderer.h"
#include "Render/IRenderResource.h"
#include "Render/MaterialManager.h"
#include "Util.h"
#include "../../RenderOutputManager.h"

#include <array>
#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>
#include <map>
#include <string>
#include <vector>

using namespace RE_HANDLE_TYPE;

namespace
{
	void RegisterGraphicsEnumString()
	{
		//Register enum string
		RegisterResourceEnumString( );

		//Fill Mode
		REGISTER_ENUM_STRING( D3D11_FILL_SOLID );
		REGISTER_ENUM_STRING( D3D11_FILL_WIREFRAME );

		//Cull Mode
		REGISTER_ENUM_STRING( D3D11_CULL_NONE );
		REGISTER_ENUM_STRING( D3D11_CULL_FRONT );
		REGISTER_ENUM_STRING( D3D11_CULL_BACK );

		//Comparision Function
		REGISTER_ENUM_STRING( D3D11_COMPARISON_NEVER );
		REGISTER_ENUM_STRING( D3D11_COMPARISON_LESS );
		REGISTER_ENUM_STRING( D3D11_COMPARISON_EQUAL );
		REGISTER_ENUM_STRING( D3D11_COMPARISON_LESS_EQUAL );
		REGISTER_ENUM_STRING( D3D11_COMPARISON_GREATER );
		REGISTER_ENUM_STRING( D3D11_COMPARISON_NOT_EQUAL );
		REGISTER_ENUM_STRING( D3D11_COMPARISON_GREATER_EQUAL );
		REGISTER_ENUM_STRING( D3D11_COMPARISON_ALWAYS );

		//Depth Write Mask
		REGISTER_ENUM_STRING( D3D11_DEPTH_WRITE_MASK_ZERO );
		REGISTER_ENUM_STRING( D3D11_DEPTH_WRITE_MASK_ALL );

		//Stencil OP
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_KEEP );
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_ZERO );
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_REPLACE );
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_INCR_SAT );
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_DECR_SAT );
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_INVERT );
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_INCR );
		REGISTER_ENUM_STRING( D3D11_STENCIL_OP_DECR );

		//Filter
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_MAG_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_MIN_MAG_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_ANISOTROPIC );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR );
		REGISTER_ENUM_STRING( D3D11_FILTER_COMPARISON_ANISOTROPIC );

		//Texture Address Mode
		REGISTER_ENUM_STRING( D3D11_TEXTURE_ADDRESS_WRAP );
		REGISTER_ENUM_STRING( D3D11_TEXTURE_ADDRESS_MIRROR );
		REGISTER_ENUM_STRING( D3D11_TEXTURE_ADDRESS_CLAMP );
		REGISTER_ENUM_STRING( D3D11_TEXTURE_ADDRESS_BORDER );
		REGISTER_ENUM_STRING( D3D11_TEXTURE_ADDRESS_MIRROR_ONCE );

		//Blend
		REGISTER_ENUM_STRING( D3D11_BLEND_ZERO );
		REGISTER_ENUM_STRING( D3D11_BLEND_ONE );
		REGISTER_ENUM_STRING( D3D11_BLEND_SRC_COLOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_INV_SRC_COLOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_SRC_ALPHA );
		REGISTER_ENUM_STRING( D3D11_BLEND_INV_SRC_ALPHA );
		REGISTER_ENUM_STRING( D3D11_BLEND_DEST_ALPHA );
		REGISTER_ENUM_STRING( D3D11_BLEND_INV_DEST_ALPHA );
		REGISTER_ENUM_STRING( D3D11_BLEND_DEST_COLOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_INV_DEST_COLOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_SRC_ALPHA_SAT );
		REGISTER_ENUM_STRING( D3D11_BLEND_BLEND_FACTOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_INV_BLEND_FACTOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_SRC1_COLOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_INV_SRC1_COLOR );
		REGISTER_ENUM_STRING( D3D11_BLEND_SRC1_ALPHA );
		REGISTER_ENUM_STRING( D3D11_BLEND_INV_SRC1_ALPHA );

		//Blend Op
		REGISTER_ENUM_STRING( D3D11_BLEND_OP_ADD );
		REGISTER_ENUM_STRING( D3D11_BLEND_OP_SUBTRACT );
		REGISTER_ENUM_STRING( D3D11_BLEND_OP_REV_SUBTRACT );
		REGISTER_ENUM_STRING( D3D11_BLEND_OP_MIN );
		REGISTER_ENUM_STRING( D3D11_BLEND_OP_MAX );

		//Color Write Enable
		REGISTER_ENUM_STRING( D3D11_COLOR_WRITE_ENABLE_RED );
		REGISTER_ENUM_STRING( D3D11_COLOR_WRITE_ENABLE_GREEN );
		REGISTER_ENUM_STRING( D3D11_COLOR_WRITE_ENABLE_BLUE );
		REGISTER_ENUM_STRING( D3D11_COLOR_WRITE_ENABLE_ALPHA );
		REGISTER_ENUM_STRING( D3D11_COLOR_WRITE_ENABLE_ALL );
	}
};

class CDirect3D11 : public IRenderer
{
public:
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;
	virtual void ShutDownRenderer( ) override;
	virtual void SceneBegin( ) override;
	virtual void ForwardRenderEnd( ) override;
	virtual BYTE SceneEnd( ) override;

	virtual RE_HANDLE CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual RE_HANDLE CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual RE_HANDLE CreateComputeShader( const TCHAR* pFilePath, const char* pProfile ) override;

	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait ) override;

	virtual void* LockBuffer( RE_HANDLE buffer, int lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) override;
	virtual void UnLockBuffer( RE_HANDLE buffer, UINT subResource = 0 ) override;

	virtual RE_HANDLE FindGraphicsShaderByName( const TCHAR* pName ) override;
	virtual RE_HANDLE FindComputeShaderByName( const TCHAR* pName ) override;

	virtual Material SearchMaterial( const TCHAR* pMaterialName ) override;

	virtual void SetViewports( const Viewport* viewPorts, int count ) override;
	virtual void SetScissorRects( const RECT* rects, int size ) override;

	virtual RE_HANDLE CreateShaderResourceFromFile( const String& fileName ) override;

	virtual RE_HANDLE CreateRasterizerState( const String& stateName ) override;
	virtual RE_HANDLE CreateSamplerState( const String& stateName ) override;
	virtual RE_HANDLE CreateDepthStencilState( const String& stateName ) override;
	virtual RE_HANDLE CreateBlendState( const String& stateName ) override;

	virtual void TakeSnapshot2D( const String& sourceTextureName, const String& destTextureName ) override;

	virtual void ClearRendertarget( RE_HANDLE renderTarget, const float( &clearColor )[4] ) override;
	virtual void ClearDepthStencil( RE_HANDLE depthStencil, float depthColor, UINT8 stencilColor ) override;

	virtual void BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets ) override;
	virtual void BindIndexBuffer( RE_HANDLE indexBuffer, UINT indexOffset ) override;
	virtual void BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, const RE_HANDLE* pConstantBuffers ) override;
	virtual void BindShader( SHADER_TYPE type, RE_HANDLE shader ) override;
	virtual void BindShaderResource( SHADER_TYPE type, int startSlot, int count, const RE_HANDLE* resource ) override;
	virtual void BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource ) override;
	virtual void BindRenderTargets( const RE_HANDLE* pRenderTargets, int renderTargetCount, RE_HANDLE depthStencil ) override;
	virtual void BindRasterizerState( RE_HANDLE rasterizerState ) override;
	virtual void BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, const RE_HANDLE* pSamplerStates ) override;
	virtual void BindDepthStencilState( RE_HANDLE depthStencilState ) override;
	virtual void BindBlendState( RE_HANDLE blendState ) override;
	virtual void BindMaterial( Material handle ) override;

	virtual void Draw( UINT primitive, UINT vertexCount, UINT vertexOffset = 0 ) override;
	virtual void DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) override;
	virtual void DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawAuto( ) override;
	virtual void Dispatch( int x, int y, int z = 1 ) override;

	virtual IResourceManager& GetResourceManager( ) override { return m_resourceManager; }

	virtual void GenerateMips( RE_HANDLE shaderResource ) override;

	virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) override;

	CDirect3D11( );
	virtual ~CDirect3D11( );
private:
	bool CreateDeviceDependentResource( HWND hWnd, UINT nWndWidth, UINT nWndHeight );
	bool CreateDeviceIndependentResource( );
	void ReportLiveDevice( );
	bool InitializeMaterial( );
	void EnumerateSampleCountAndQuality( int* size, DXGI_SAMPLE_DESC* pSamples );

	Microsoft::WRL::ComPtr<ID3D11Device>					m_pd3d11Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pd3d11DeviceContext;
	Microsoft::WRL::ComPtr<IDXGIFactory1>					m_pdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain>					m_pdxgiSwapChain;

	CD3D11ResourceManager									m_resourceManager;
	CRenderOutputManager									m_renderOutput;

	CD3D11RenderStateManager								m_renderStateManager;
	CMaterialManager										m_materialManager;

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
	m_resourceManager.AppSizeChanged( nWndWidth, nWndHeight );

	if ( !InitializeMaterial( ) )
	{
		return false;
	}

	if ( !m_renderOutput.Initialize( m_resourceManager, *m_pdxgiSwapChain.Get( ), m_multiSampleOption ) )
	{
		return false;
	}

	return true;
}

void CDirect3D11::HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight )
{
	m_resourceManager.OnDeviceLost( );
	m_materialManager.OnDeviceLost( );

	m_pdxgiSwapChain.Reset( );
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

	if ( !InitializeMaterial( ) )
	{
		__debugbreak( );
	}

	m_resourceManager.AppSizeChanged( nWndWidth, nWndHeight );

	if ( !m_renderOutput.Initialize( m_resourceManager, *m_pdxgiSwapChain.Get( ), m_multiSampleOption ) )
	{
		__debugbreak( );
	}
}

void CDirect3D11::AppSizeChanged( UINT nWndWidth, UINT nWndHeight )
{
	if ( m_pd3d11Device == nullptr )
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

	m_renderOutput.AppSizeChanged( m_resourceManager, *m_pdxgiSwapChain.Get(), m_multiSampleOption );
}

void CDirect3D11::ShutDownRenderer( )
{
#ifdef _DEBUG
	ReportLiveDevice( );
#endif
}

void CDirect3D11::SceneBegin( )
{
	m_renderOutput.SetRenderTargetDepthStencilView( *this );
	m_renderOutput.ClearDepthStencil( *this );
	m_renderOutput.ClearRenderTargets( *this, { 1.0f, 1.0f, 1.0f, 1.0f } );
}

void CDirect3D11::ForwardRenderEnd( )
{
	TakeSnapshot2D( _T( "DepthGBuffer" ), _T( "DuplicateDepthGBuffer" ) );
	TakeSnapshot2D( _T( "DefaultRenderTarget" ), _T( "DuplicateFrameBuffer" ) );
}

BYTE CDirect3D11::SceneEnd( )
{
	HRESULT hr = m_pdxgiSwapChain->Present( 0, 0 );

	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{
		return DEVICE_ERROR::DEVICE_LOST;
	}

#ifdef _DEBUG
	TakeSnapshot2D( _T( "DefaultDepthStencil" ), _T( "DebugDepthStencil" ) );
	TakeSnapshot2D( _T( "ShadowMap" ), _T( "DebugShadowMap" ) );
#endif

	return DEVICE_ERROR::NONE;
}

RE_HANDLE CDirect3D11::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile )
{
	return m_resourceManager.CreateVertexShader( pFilePath, pProfile );
}

RE_HANDLE CDirect3D11::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	return m_resourceManager.CreatePixelShader( pFilePath, pProfile );
}

RE_HANDLE CDirect3D11::CreateComputeShader( const TCHAR* pFilePath, const char* pProfile )
{
	return m_resourceManager.CreateComputeShader( pFilePath, pProfile );
}

RE_HANDLE CDirect3D11::CreateBuffer( const BUFFER_TRAIT& trait )
{
	return m_resourceManager.CreateBuffer( trait );
}

void* CDirect3D11::LockBuffer( RE_HANDLE buffer, int lockFlag, UINT subResource )
{
	const CD3D11Buffer& d3d11buffer = m_resourceManager.GetBuffer( buffer );
	D3D11_MAPPED_SUBRESOURCE resource;

	HRESULT hr = m_pd3d11DeviceContext->Map( d3d11buffer.Get( ), subResource, ConvertLockFlagToD3D11Map( lockFlag ), 0, &resource );
	if ( FAILED( hr ) )
	{
		__debugbreak();
	}

	return resource.pData;
}

void CDirect3D11::UnLockBuffer( RE_HANDLE buffer, UINT subResource )
{
	const CD3D11Buffer& d3d11buffer = m_resourceManager.GetBuffer( buffer );

	m_pd3d11DeviceContext->Unmap( d3d11buffer.Get( ), subResource );
}

RE_HANDLE CDirect3D11::FindGraphicsShaderByName( const TCHAR* pName )
{
	return m_resourceManager.FindGraphicsShaderByName( pName );
}

RE_HANDLE CDirect3D11::FindComputeShaderByName( const TCHAR* pName )
{
	return m_resourceManager.FindComputeShaderByName( pName );
}

bool CDirect3D11::InitializeMaterial( )
{
	return m_materialManager.LoadMaterials( *this );
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

Material CDirect3D11::SearchMaterial( const TCHAR* pMaterialName )
{
	return m_materialManager.SearchMaterialByName( pMaterialName );
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

	m_pd3d11DeviceContext->RSSetViewports( d3d11Viewports.size( ), d3d11Viewports.data( ) );
}

void CDirect3D11::SetScissorRects( const RECT* rects, int size )
{
	m_pd3d11DeviceContext->RSSetScissorRects( size, rects );
}

RE_HANDLE CDirect3D11::CreateShaderResourceFromFile( const String& fileName )
{
	return m_resourceManager.CreateShaderResourceFromFile( fileName );
}

RE_HANDLE CDirect3D11::CreateRasterizerState( const String& stateName )
{
	return m_resourceManager.CreateRasterizerState( stateName );
}

RE_HANDLE CDirect3D11::CreateSamplerState( const String& stateName )
{
	return m_resourceManager.CreateSamplerState( stateName );
}

RE_HANDLE CDirect3D11::CreateDepthStencilState( const String& stateName )
{
	return m_resourceManager.CreateDepthStencilState( stateName );
}

RE_HANDLE CDirect3D11::CreateBlendState( const String& stateName )
{
	return m_resourceManager.CreateBlendState( stateName );
}

void CDirect3D11::TakeSnapshot2D( const String& sourceTextureName, const String& destTextureName )
{
	m_resourceManager.TakeSnapshot( sourceTextureName, destTextureName );
}

void CDirect3D11::ClearRendertarget( RE_HANDLE renderTarget, const float (&clearColor)[4] )
{
	const CRenderTarget& rtv = m_resourceManager.GetRendertarget( renderTarget );
	m_pd3d11DeviceContext->ClearRenderTargetView( rtv.Get(), clearColor );
}

void CDirect3D11::ClearDepthStencil( RE_HANDLE depthStencil, float depthColor, UINT8 stencilColor )
{
	const CDepthStencil& dsv = m_resourceManager.GetDepthstencil( depthStencil );
	m_pd3d11DeviceContext->ClearDepthStencilView( dsv.Get( ), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
}

void CDirect3D11::BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets )
{
	ID3D11Buffer* buffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( pVertexBuffers[i] != INVALID_HANDLE )
		{
			const CD3D11Buffer& d3d11buffer = m_resourceManager.GetBuffer( pVertexBuffers[i] );
			buffers[i] = d3d11buffer.Get( );
		}
	}

	m_renderStateManager.SetVertexBuffer( startSlot, numBuffers, buffers, pStrides, pOffsets );
}

void CDirect3D11::BindIndexBuffer( RE_HANDLE indexBuffer, UINT indexOffset )
{
	ID3D11Buffer* buffer = nullptr;
	DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;

	if ( indexBuffer != INVALID_HANDLE )
	{
		const CD3D11Buffer& d3d11buffer = m_resourceManager.GetBuffer( indexBuffer );
		buffer = d3d11buffer.Get( );
		
		if ( d3d11buffer.Stride() == 4 )
		{
			format = DXGI_FORMAT_R32_UINT;
		}
	}

	m_renderStateManager.SetIndexBuffer( buffer, format, indexOffset );
}

void CDirect3D11::BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, const RE_HANDLE* pConstantBuffers )
{
	ID3D11Buffer* buffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		if ( pConstantBuffers[i] != INVALID_HANDLE )
		{
			const CD3D11Buffer& d3d11buffer = m_resourceManager.GetBuffer( pConstantBuffers[i] );
			buffers[i] = d3d11buffer.Get( );
		}
	}

	switch ( type )
	{
	case VS:
		m_renderStateManager.SetVsConstantBuffers( startSlot, numBuffers, buffers );
		break;
	case HS:
		break;
	case DS:
		break;
	case GS:
		break;
	case PS:
		m_renderStateManager.SetPsConstantBuffers( startSlot, numBuffers, buffers );
		break;
	case CS:
		m_renderStateManager.SetCsConstantBuffers( startSlot, numBuffers, buffers );
		break;
	default:
		break;
	}
}

void CDirect3D11::BindShader( SHADER_TYPE type, RE_HANDLE shader )
{
	switch ( type )
	{
	case VS:
		{
			ID3D11VertexShader* vs = nullptr;
			ID3D11InputLayout* inputLayout = nullptr;
			if ( shader != INVALID_HANDLE )
			{
				const CD3D11VertexShader& d3d11vs = m_resourceManager.GetVertexShader( shader );
				vs = d3d11vs.Get( );
				inputLayout = d3d11vs.GetLayout( );
			}

			m_renderStateManager.SetVertexShader( vs, nullptr, 0 );
			m_renderStateManager.SetInputLayout( inputLayout );
		}
		break;
	case HS:
		break;
	case DS:
		break;
	case GS:
		break;
	case PS:
		{
			ID3D11PixelShader* ps = nullptr;
			if ( shader != INVALID_HANDLE )
			{
				const CD3D11PixelShader& d3d11ps = m_resourceManager.GetPixelShader( shader );
				ps = d3d11ps.Get( );
			}

			m_renderStateManager.SetPixelShader( ps, nullptr, 0 );
		}
		break;
	case CS:
		{
			ID3D11ComputeShader* cs = nullptr;
			if ( shader != INVALID_HANDLE )
			{
				const CD3D11ComputeShader& d3d11cs = m_resourceManager.GetComputeShader( shader );
				cs = d3d11cs.Get( );
			}
			
			m_renderStateManager.SetComputeShader( cs, nullptr, 0 );
		}
		break;
	default:
		__debugbreak( );
		break;
	}
}

void CDirect3D11::BindShaderResource( SHADER_TYPE type, int startSlot, int count, const RE_HANDLE* resource )
{
	ID3D11ShaderResourceView* srvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

	for ( int i = 0; i < count; ++i )
	{
		if ( resource[i] != INVALID_HANDLE )
		{
			const CD3D11ShaderResource& d3d11srv = m_resourceManager.GetShaderResource( resource[i] );
			srvs[i] = d3d11srv.Get( );
		}
	}

	switch ( type )
	{
	case VS:
		m_renderStateManager.SetVsShaderResource( startSlot, count, srvs );
		break;
	case HS:
		break;
	case DS:
		break;
	case GS:
		break;
	case PS:
		m_renderStateManager.SetPsShaderResource( startSlot, count, srvs );
		break;
	case CS:
		m_renderStateManager.SetCsShaderResource( startSlot, count, srvs );
		break;
	default:
		__debugbreak( );
		break;
	}
}

void CDirect3D11::BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource )
{
	ID3D11UnorderedAccessView* ravs[64] = {};
	UINT initialCounts[64] = {};

	for ( int i = 0; i < count; ++i )
	{
		if ( resource[i] != INVALID_HANDLE )
		{
			const CD3D11RandomAccessResource& d3d11rav = m_resourceManager.GetRandomAccess( resource[i] );
			ravs[i] = d3d11rav.Get( );
		}
	}

	m_renderStateManager.SetCsUnorderedAccess( startSlot, count, ravs, initialCounts );
}

void CDirect3D11::BindRenderTargets( const RE_HANDLE* pRenderTargets, int renderTargetCount, RE_HANDLE depthStencil )
{
	ID3D11RenderTargetView* rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

	for ( int i = 0; i < renderTargetCount; ++i )
	{
		if ( pRenderTargets[i] == INVALID_HANDLE )
		{
			rtvs[i] = nullptr;
		}
		else
		{
			rtvs[i] = m_resourceManager.GetRendertarget( pRenderTargets[i] ).Get( );
		}
	}
	
	ID3D11DepthStencilView* dsv = nullptr;
	if ( depthStencil != INVALID_HANDLE )
	{
		dsv = m_resourceManager.GetDepthstencil( depthStencil ).Get();
	}
	m_renderStateManager.SetRenderTargets( static_cast<UINT>( renderTargetCount ), rtvs, dsv );
}

void CDirect3D11::BindRasterizerState( RE_HANDLE rasterizerState )
{
	ID3D11RasterizerState* state = nullptr;

	if ( rasterizerState != INVALID_HANDLE )
	{
		state = m_resourceManager.GetRasterizerState( rasterizerState ).Get();
	}

	m_renderStateManager.SetRasterizerState( state );
}

void CDirect3D11::BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, const RE_HANDLE* pSamplerStates )
{
	ID3D11SamplerState* states[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};

	for ( int i = 0; i < numSamplers; ++i )
	{
		if ( pSamplerStates[i] == INVALID_HANDLE )
		{
			states[i] = nullptr;
		}
		else
		{
			states[i] = m_resourceManager.GetSamplerState( pSamplerStates[i] ).Get( );
		}
	}

	switch ( type )
	{
	case VS:
		{
			m_renderStateManager.SetVsSamplers( startSlot, numSamplers, states );
		}
		break;
	case HS:
		{
			m_renderStateManager.SetHsSamplers( startSlot, numSamplers, states );
		}
		break;
	case DS:
		{
			m_renderStateManager.SetDsSamplers( startSlot, numSamplers, states );
		}
		break;
	case GS:
		{
			m_renderStateManager.SetGsSamplers( startSlot, numSamplers, states );
		}
		break;
	case PS:
		{
			m_renderStateManager.SetPsSamplers( startSlot, numSamplers, states );
		}
		break;
	case CS:
		{
			m_renderStateManager.SetCsSamplers( startSlot, numSamplers, states );
		}
		break;
	default:
		__debugbreak( );
		break;
	}
}

void CDirect3D11::BindDepthStencilState( RE_HANDLE depthStencilState )
{
	if ( depthStencilState == INVALID_HANDLE )
	{
		m_renderStateManager.SetDepthStencilState( nullptr, 0 );
	}
	else
	{
		const CD3D11DepthStencilState& state = m_resourceManager.GetDepthStencilState( depthStencilState );
		m_renderStateManager.SetDepthStencilState( state.Get( ), state.GetStencilRef( ) );
	}
}

void CDirect3D11::BindBlendState( RE_HANDLE blendState )
{
	if ( blendState == INVALID_HANDLE )
	{
		float defaultBlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		m_renderStateManager.SetBlendState( nullptr, defaultBlendFactor, D3D11_DEFAULT_SAMPLE_MASK );
	}
	else
	{
		const CD3D11BlendState& state = m_resourceManager.GetBlendState( blendState );
		m_renderStateManager.SetBlendState( state.Get( ), state.GetBlendFactor( ), state.GetSamplerMask( ) );
	}
}

void CDirect3D11::BindMaterial( Material handle )
{
	const CMaterial& mtl = m_materialManager.GetConcrete( handle );
	mtl.Bind( *this );
}

void CDirect3D11::Draw( UINT primitive, UINT vertexCount, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_renderStateManager.SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->Draw( vertexCount, vertexOffset );
}

void CDirect3D11::DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_renderStateManager.SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawIndexed( indexCount, indexOffset, vertexOffset );
}

void CDirect3D11::DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_renderStateManager.SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawInstanced( vertexCount, instanceCount, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_renderStateManager.SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawIndexedInstanced( indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawAuto( )
{
	m_pd3d11DeviceContext->DrawAuto( );
}

void CDirect3D11::Dispatch( int x, int y, int z )
{
	m_pd3d11DeviceContext->Dispatch( static_cast<UINT>( x ), static_cast<UINT>( y ), static_cast<UINT>( z ) );
}

void CDirect3D11::GenerateMips( RE_HANDLE shaderResource )
{
	const CD3D11ShaderResource& srv = m_resourceManager.GetShaderResource( shaderResource );
	m_pd3d11DeviceContext->GenerateMips( srv.Get( ) );
}

void CDirect3D11::GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option )
{
	assert( option != nullptr );
	option->m_count = m_multiSampleOption.Count;
	option->m_quality = m_multiSampleOption.Quality;
}

CDirect3D11::CDirect3D11( )
{
	RegisterGraphicsEnumString( );
}

CDirect3D11::~CDirect3D11( )
{
	ShutDownRenderer( );
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

			hr = m_pdxgiFactory->CreateSwapChain( m_pd3d11Device.Get( ), &dxgiSwapchainDesc, m_pdxgiSwapChain.GetAddressOf( ) );
			if ( FAILED( hr ) )
			{
				return false;
			}

			m_resourceManager.OnDeviceRestore( m_pd3d11Device.Get( ), m_pd3d11DeviceContext.Get( ) );
			m_renderStateManager.OnDeviceRestore( m_pd3d11DeviceContext.Get( ) );

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

IRenderer* CreateDirect3D11Renderer( )
{
	return new CDirect3D11( );
}