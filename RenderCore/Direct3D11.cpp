#include "stdafx.h"
#include "common.h"

#include "BaseMesh.h"

#include "D3D11IndexBuffer.h"
#include "D3D11PixelShader.h"
#include "D3D11VertexBuffer.h"
#include "D3D11VertexShader.h"

#include "DebugMesh.h"

#include "Direct3D11.h"

#include "MaterialSystem.h"

#include "TextureMaterial.h"
#include "TutorialMaterial.h"
#include "WireFrame.h"

#include <tuple>
#include <string>

#include "VSConstantBufferDefine.h"
#include "util_rendercore.h"

#include "../shared/Util.h"

namespace
{
	IRenderer* CreateDirect3D11Renderer( )
	{
		static CDirect3D11 direct3D11;
		return &direct3D11;
	}
};

bool CDirect3D11::InitializeRenderer( HWND hWind, UINT nWndWidth, UINT nWndHeight )
{
	ON_FAIL_RETURN( CreateD3D11Device( hWind, nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN( CreatePrimeRenderTargetVIew( ) );
	ON_FAIL_RETURN( CreatePrimeDepthBuffer( nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN( SetRenderTargetAndDepthBuffer( ) );
	m_pView = std::make_unique<RenderView>( );
	ON_FAIL_RETURN( m_pView->initialize( m_pd3d11Device.Get( ) ) );
	ON_FAIL_RETURN( m_meshLoader.Initialize( ) );
	m_worldMatrixBuffer = CreateConstantBuffer( sizeof( D3DXMATRIX ), 1, NULL );

	if ( !m_worldMatrixBuffer )
	{
		return false;
	}

	return true;
}

void CDirect3D11::ShutDownRenderer( )
{
	m_pView = std::move( nullptr );

	FOR_EACH_MAP( m_shaderList, i )
	{
		SAFE_DELETE( i->second );
	}
	m_shaderList.clear( );

	FOR_EACH_VEC( m_bufferList, j )
	{
		SAFE_DELETE( *j );
	}
	m_bufferList.clear( );

#ifdef _DEBUG
	ReportLiveDevice( );
#endif
}

void CDirect3D11::ClearRenderTargetView( )
{
	ClearRenderTargetView( 1.0f, 1.0f, 1.0f, 1.0f );
}

void CDirect3D11::ClearRenderTargetView( float r, float g, float b, float a )
{
	const float clearColor[4] = { r, g, b, a };

	m_pd3d11DeviceContext->ClearRenderTargetView( m_pd3d11PrimeRTView.Get( ), clearColor );
}

void CDirect3D11::ClearDepthStencilView( )
{
	m_pd3d11DeviceContext->ClearDepthStencilView( m_pd3d11PrimeDSView.Get( ), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

void CDirect3D11::SceneBegin( )
{
	ClearDepthStencilView( );
	ClearRenderTargetView( );
	if ( m_pView )
	{
		m_pView->UpdataView( m_pd3d11DeviceContext.Get( ) );
	}
}

void CDirect3D11::SceneEnd( )
{
	m_pdxgiSwapChain->Present( 0, 0 );
}

IShader* CDirect3D11::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile )
{
	D3D11VertexShader* vs = new D3D11VertexShader( );

	D3D11_INPUT_ELEMENT_DESC* inputDesc = vs->CreateInputElementDesc( 4 );

	inputDesc[0].SemanticName = "POSITION";
	inputDesc[0].SemanticIndex = 0;
	inputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[0].InputSlot = 0;
	inputDesc[0].AlignedByteOffset = 0;
	inputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[0].InstanceDataStepRate = 0;

	inputDesc[1].SemanticName = "NORMAL";
	inputDesc[1].SemanticIndex = 0;
	inputDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[1].InputSlot = 0;
	inputDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[1].InstanceDataStepRate = 0;

	inputDesc[2].SemanticName = "COLOR";
	inputDesc[2].SemanticIndex = 0;
	inputDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[2].InputSlot = 0;
	inputDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[2].InstanceDataStepRate = 0;

	inputDesc[3].SemanticName = "TEXCOORD";
	inputDesc[3].SemanticIndex = 0;
	inputDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputDesc[3].InputSlot = 0;
	inputDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[3].InstanceDataStepRate = 0;

	if ( vs->CreateShader( m_pd3d11Device.Get( ), pFilePath, pProfile ) )
	{
		m_shaderList.emplace( UTIL::GetFileName( pFilePath ), vs );
		return vs;
	}

	SAFE_DELETE( vs );
	return nullptr;
}

IShader* CDirect3D11::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	D3D11PixelShader* ps = new D3D11PixelShader( );
	if ( ps->CreateShader( m_pd3d11Device.Get( ), pFilePath, pProfile ) )
	{
		m_shaderList.insert( std::pair<String, IShader*>( UTIL::GetFileName( pFilePath ), ps ) );
		return ps;
	}

	SAFE_DELETE( ps );
	return nullptr;
}

IBuffer* CDirect3D11::CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData )
{
	D3D11VertexBuffer* vb = new D3D11VertexBuffer( );
	if ( vb->CreateBuffer( m_pd3d11Device.Get( ), stride, numOfElement, srcData ) )
	{
		m_bufferList.emplace_back( vb );
		return vb;
	}

	SAFE_DELETE( vb );
	return nullptr;
}

IBuffer* CDirect3D11::CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData )
{
	D3D11IndexBuffer* ib = new D3D11IndexBuffer( );
	if ( ib->CreateBuffer( m_pd3d11Device.Get( ), stride, numOfElement, srcData ) )
	{
		m_bufferList.emplace_back( ib );
		return ib;
	}

	SAFE_DELETE( ib );
	return nullptr;
}

IBuffer* CDirect3D11::CreateConstantBuffer( const UINT stride, const UINT numOfElement, const void* srcData )
{
	D3D11ConstantBuffer* cb = new D3D11ConstantBuffer( );
	if ( cb->CreateBuffer( m_pd3d11Device.Get( ), stride, numOfElement, srcData ) )
	{
		m_bufferList.emplace_back( cb );
		return cb;
	}

	SAFE_DELETE( cb );
	return nullptr;
}


IShader* CDirect3D11::SearchShaderByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	std::map<String, IShader*>::iterator finded = m_shaderList.find( pName );

	if ( finded != m_shaderList.end( ) )
	{
		return finded->second;
	}

	return nullptr;
}

bool CDirect3D11::InitMaterial( )
{
	REGISTER_MATERIAL( TutorialMaterial, tutorial );
	REGISTER_MATERIAL( WireFrame, wireframe );
	REGISTER_MATERIAL( TextureMaterial, texture );

	return true;
}

std::shared_ptr<IMaterial> CDirect3D11::GetMaterialPtr( const TCHAR* pMaterialName )
{
	return MaterialSystem::GetInstance( )->SearchMaterialByName( pMaterialName );
}

std::shared_ptr<IMesh> CDirect3D11::GetModelPtr( const TCHAR* pModelName )
{
	if ( m_meshLoader.LoadMeshFromFile( pModelName ) )
	{
		return m_meshLoader.GetMesh( pModelName );
	}

	return nullptr;
}

void CDirect3D11::DrawModel( std::shared_ptr<IMesh> pModel )
{
	if ( pModel )
	{
		pModel->Draw( m_pd3d11DeviceContext.Get( ) );
	}
}

void CDirect3D11::PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth, const float maxDepth )
{
	if ( m_pView )
	{
		m_pView->PushViewPort( topLeftX, topLeftY, width, height, minDepth, maxDepth );
		m_pView->SetViewPort( m_pd3d11DeviceContext.Get( ) );
	}
}

void CDirect3D11::PopViewPort( )
{
	if ( m_pView )
	{
		m_pView->PopViewPort( );
		m_pView->SetViewPort( m_pd3d11DeviceContext.Get( ) );
	}
}

IRenderView* CDirect3D11::GetCurrentRenderView( )
{
	return m_pView.get( );
}

void CDirect3D11::UpdateWorldMatrix( const D3DXMATRIX& worldMatrix )
{
	D3DXMATRIX* pWorld = static_cast<D3DXMATRIX*>( m_worldMatrixBuffer->LockBuffer( m_pd3d11DeviceContext.Get( ) ) );

	if ( pWorld )
	{
		D3DXMATRIX transposWorld;

		D3DXMatrixTranspose( &transposWorld, &worldMatrix );

		CopyMemory( pWorld, &transposWorld, sizeof( D3DXMATRIX ) );
	}

	m_worldMatrixBuffer->UnLockBuffer( m_pd3d11DeviceContext.Get( ) );
	m_worldMatrixBuffer->SetVSBuffer( m_pd3d11DeviceContext.Get( ), static_cast<int>( VS_CONSTANT_BUFFER::WORLD ) );
}

ID3D11RasterizerState* CDirect3D11::CreateRenderState( bool isWireFrame, bool isAntialiasedLine )
{
	ID3D11RasterizerState* rsState = nullptr;
	D3D11_RASTERIZER_DESC desc;
	::ZeroMemory( &desc, sizeof( D3D11_RASTERIZER_DESC ) );

	desc.FillMode = isWireFrame ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	desc.AntialiasedLineEnable = isAntialiasedLine ? true : false;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthClipEnable = true;


	m_pd3d11Device->CreateRasterizerState( &desc, &rsState );

	return rsState;
}

std::shared_ptr<ITexture> CDirect3D11::GetTextureFromFile( const String& fileName )
{
	m_textureManager.LoadTextureFromFile( m_pd3d11Device.Get( ), fileName );
	return m_textureManager.GetTexture( fileName );
}

ID3D11SamplerState* CDirect3D11::CreateSampler( )
{
	ID3D11SamplerState* pSampler;

	D3D11_SAMPLER_DESC desc;
	::ZeroMemory( &desc, sizeof( D3D11_SAMPLER_DESC ) );

	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.BorderColor[0] = 1.f;
	desc.BorderColor[1] = 1.f;
	desc.BorderColor[2] = 1.f;
	desc.BorderColor[3] = 1.f;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 1;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = -FLT_MAX;

	m_pd3d11Device->CreateSamplerState( &desc, &pSampler );

	return pSampler;
}

bool CDirect3D11::CreateD3D11Device( HWND hWind, UINT nWndWidth, UINT nWndHeight )
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

	DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc;
	::ZeroMemory( &dxgiSwapchainDesc, sizeof( dxgiSwapchainDesc ) );

	dxgiSwapchainDesc.BufferCount = 1;
	dxgiSwapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapchainDesc.BufferDesc.Height = nWndHeight;
	dxgiSwapchainDesc.BufferDesc.Width = nWndWidth;
	dxgiSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapchainDesc.OutputWindow = hWind;
	dxgiSwapchainDesc.SampleDesc.Count = 1;
	dxgiSwapchainDesc.SampleDesc.Quality = 0;
	dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dxgiSwapchainDesc.Windowed = true;

	D3D_FEATURE_LEVEL selectedFeature = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr;

	for ( int i = 0; i < _countof( d3dDriverTypes ); ++i )
	{
		if ( SUCCEEDED( hr = D3D11CreateDeviceAndSwapChain( nullptr,
			d3dDriverTypes[i],
			nullptr,
			flag,
			d3dFeatureLevel,
			_countof( d3dFeatureLevel ),
			D3D11_SDK_VERSION,
			&dxgiSwapchainDesc,
			&m_pdxgiSwapChain,
			&m_pd3d11Device,
			&selectedFeature,
			&m_pd3d11DeviceContext
			) ) )
		{
#ifdef _DEBUG
			SetDebugName( m_pd3d11DeviceContext.Get( ), "Device Context" );
#endif

			return true;
		}
	}

	return false;
}

bool CDirect3D11::CreatePrimeRenderTargetVIew( )
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pd3d11BackBuffer;

	if ( SUCCEEDED( m_pdxgiSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pd3d11BackBuffer ) ) )
	{
		HRESULT hr = m_pd3d11Device->CreateRenderTargetView( pd3d11BackBuffer.Get( ), nullptr, &m_pd3d11PrimeRTView );

		if ( SUCCEEDED( hr ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::CreatePrimeDepthBuffer( UINT nWndWidth, UINT nWndHeight )
{
	D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
	::ZeroMemory( &d3d11Texture2DDesc, sizeof( d3d11Texture2DDesc ) );

	d3d11Texture2DDesc.ArraySize = 1;
	d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3d11Texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d11Texture2DDesc.Height = nWndHeight;
	d3d11Texture2DDesc.Width = nWndWidth;
	d3d11Texture2DDesc.MipLevels = 1;
	d3d11Texture2DDesc.SampleDesc.Count = 1;
	d3d11Texture2DDesc.SampleDesc.Quality = 0;
	d3d11Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;

	if ( SUCCEEDED( m_pd3d11Device->CreateTexture2D( &d3d11Texture2DDesc, nullptr, &m_pd3d11PrimeDSBuffer ) ) )
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DSDesc;
		::ZeroMemory( &d3d11DSDesc, sizeof( d3d11DSDesc ) );

		d3d11DSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3d11DSDesc.Texture2D.MipSlice = 0;
		d3d11DSDesc.Format = d3d11Texture2DDesc.Format;

		if ( SUCCEEDED( m_pd3d11Device->CreateDepthStencilView( m_pd3d11PrimeDSBuffer.Get( ), &d3d11DSDesc, &m_pd3d11PrimeDSView ) ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::SetRenderTargetAndDepthBuffer( )
{
	if ( m_pd3d11PrimeRTView && m_pd3d11PrimeDSView )
	{
		m_pd3d11DeviceContext->OMSetRenderTargets( 1, m_pd3d11PrimeRTView.GetAddressOf( ), m_pd3d11PrimeDSView.Get( ) );
		return true;
	}
	else
	{
		return false;
	}
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

CDirect3D11::CDirect3D11( ) : m_pd3d11Device( nullptr ),
m_pd3d11DeviceContext( nullptr ),
m_pdxgiSwapChain( nullptr ),
m_pd3d11PrimeRTView( nullptr ),
m_pd3d11PrimeDSBuffer( nullptr ),
m_pd3d11PrimeDSView( nullptr ),
m_worldMatrixBuffer( nullptr )
{
}


CDirect3D11::~CDirect3D11( )
{
	ShutDownRenderer( );
}