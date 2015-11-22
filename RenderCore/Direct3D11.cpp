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

#include "TutorialMaterial.h"

#include <string>

#include "VSConstantBufferDefine.h"

namespace
{
	IRenderer* CreateDirect3D11Renderer ( )
	{
		static CDirect3D11 direct3D11;
		return &direct3D11;
	}

	String GetFileName( const TCHAR* pFilePath )
	{
		const TCHAR* start = _tcsrchr( pFilePath, _T('/') );
		const TCHAR* end = _tcsrchr( pFilePath, _T( '.' ) );

		return String( start + 1, end );
	}
};

bool CDirect3D11::InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight )
{
	ON_FAIL_RETURN ( CreateD3D11Device ( hWind, nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN ( CreatePrimeRenderTargetVIew () );
	ON_FAIL_RETURN ( CreatePrimeDepthBuffer ( nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN ( SetRenderTargetAndDepthBuffer () );
	ON_FAIL_RETURN ( m_view.initialize( m_pd3d11Device ) );
	m_worldMatrixBuffer = CreateConstantBuffer( sizeof( D3DXMATRIX ), 1, NULL );

	if ( !m_worldMatrixBuffer )
	{
		return false;
	}

	return true;
}

void CDirect3D11::ShutDownRenderer ( )
{
	FOR_EACH_MAP ( m_shaderList, i )
	{
		SAFE_DELETE ( i->second );
	}
	m_shaderList.clear();

	FOR_EACH_VEC ( m_bufferList, j )
	{
		SAFE_DELETE( j._Ptr );
	}
	m_bufferList.clear( );

	FOR_EACH_VEC( m_models, k )
	{
		SAFE_DELETE( k._Ptr );
	}
	m_models.clear( );

	SAFE_RELEASE ( m_pd3d11DeviceContext );
	SAFE_RELEASE ( m_pd3d11Device );
	SAFE_RELEASE ( m_pdxgiSwapChain );
	SAFE_RELEASE ( m_pd3d11PrimeRTView );
	SAFE_RELEASE ( m_pd3d11PrimeDSBuffer );
	SAFE_RELEASE ( m_pd3d11PrimeDSView );
}

void CDirect3D11::ClearRenderTargetView ( )
{
	ClearRenderTargetView ( 1.0f, 1.0f, 1.0f, 1.0f );
}

void CDirect3D11::ClearRenderTargetView ( float r, float g, float b, float a )
{
	const float clearColor[4] = { r, g, b, a };

	m_pd3d11DeviceContext->ClearRenderTargetView ( m_pd3d11PrimeRTView, clearColor );
}

void CDirect3D11::ClearDepthStencilView ( )
{
	m_pd3d11DeviceContext->ClearDepthStencilView ( m_pd3d11PrimeDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

void CDirect3D11::SceneBegin( )
{
	ClearDepthStencilView( );
	ClearRenderTargetView( );
	m_view.UpdataView( m_pd3d11DeviceContext );
}

void CDirect3D11::SceneEnd( )
{
	m_pdxgiSwapChain->Present ( 0, 0 );
}

IShader* CDirect3D11::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile )
{
	D3D11VertexShader* vs = new D3D11VertexShader( );
	
	D3D11_INPUT_ELEMENT_DESC* inputDesc = vs->CreateInputElementDesc( 1 );

	inputDesc[0].SemanticName = "POSITION";
	inputDesc[0].SemanticIndex = 0;
	inputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[0].InputSlot = 0;
	inputDesc[0].AlignedByteOffset = 0;
	inputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[0].InstanceDataStepRate = 0;

	if ( vs->CreateShader( m_pd3d11Device, pFilePath, pProfile ) )
	{
		m_shaderList.emplace( GetFileName(pFilePath), vs );
		return vs;
	}

	SAFE_DELETE( vs );
	return nullptr;
}

IShader* CDirect3D11::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	D3D11PixelShader* ps = new D3D11PixelShader( );
	if ( ps->CreateShader( m_pd3d11Device, pFilePath, pProfile ) )
	{
		m_shaderList.insert( std::pair<String, IShader*>( GetFileName( pFilePath ), ps ) );
		return ps;
	}

	SAFE_DELETE( ps );
	return nullptr;
}

IBuffer* CDirect3D11::CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData )
{
	D3D11VertexBuffer* vb = new D3D11VertexBuffer( );
	if ( vb->CreateBuffer( m_pd3d11Device, stride, numOfElement, srcData ) )
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
	if ( ib->CreateBuffer( m_pd3d11Device, stride, numOfElement, srcData ) )
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
	if ( cb->CreateBuffer( m_pd3d11Device, stride, numOfElement, srcData ) )
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

	return true;
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
		pModel->Draw( m_pd3d11DeviceContext );
	}
}

void CDirect3D11::PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth, const float maxDepth )
{
	m_view.PushViewPort( topLeftX, topLeftY, width, height, minDepth, maxDepth );
	m_view.SetViewPort( m_pd3d11DeviceContext );
}

void CDirect3D11::PopViewPort( )
{
	m_view.PopViewPort( );
	m_view.SetViewPort( m_pd3d11DeviceContext );
}

IRenderView* CDirect3D11::GetCurrentRenderView( )
{
	return &m_view;
}

void CDirect3D11::UpdateWorldMatrix( const D3DXMATRIX& worldMatrix )
{
	D3DXMATRIX* pWorld = static_cast<D3DXMATRIX*>( m_worldMatrixBuffer->LockBuffer( m_pd3d11DeviceContext ) );

	if ( pWorld )
	{
		D3DXMATRIX transposWorld;

		D3DXMatrixTranspose( &transposWorld, &worldMatrix );

		CopyMemory( pWorld, &transposWorld, sizeof( D3DXMATRIX ) );
	}

	m_worldMatrixBuffer->UnLockBuffer( m_pd3d11DeviceContext );
	m_worldMatrixBuffer->SetVSBuffer( m_pd3d11DeviceContext, static_cast<int>( VS_CONSTANT_BUFFER::WORLD ) );
}

bool CDirect3D11::CreateD3D11Device ( HWND hWind, UINT nWndWidth, UINT nWndHeight )
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
	::ZeroMemory ( &dxgiSwapchainDesc, sizeof(dxgiSwapchainDesc) );

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

	for ( int i = 0; i < _countof ( d3dDriverTypes ); ++i )
	{
		if ( SUCCEEDED( hr = D3D11CreateDeviceAndSwapChain( nullptr,
			d3dDriverTypes[i],
			nullptr,
			flag,
			d3dFeatureLevel,
			_countof ( d3dFeatureLevel ),
			D3D11_SDK_VERSION,
			&dxgiSwapchainDesc,
			&m_pdxgiSwapChain,
			&m_pd3d11Device,
			&selectedFeature,
			&m_pd3d11DeviceContext
			) ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::CreatePrimeRenderTargetVIew ( )
{
	ID3D11Texture2D* pd3d11BackBuffer;

	if ( SUCCEEDED ( m_pdxgiSwapChain->GetBuffer ( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pd3d11BackBuffer ) ) )
	{
		HRESULT hr = m_pd3d11Device->CreateRenderTargetView( pd3d11BackBuffer, nullptr, &m_pd3d11PrimeRTView );

		SAFE_RELEASE ( pd3d11BackBuffer );

		if ( SUCCEEDED ( hr ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::CreatePrimeDepthBuffer ( UINT nWndWidth, UINT nWndHeight )
{
	D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
	::ZeroMemory ( &d3d11Texture2DDesc, sizeof(d3d11Texture2DDesc) );

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
		::ZeroMemory ( &d3d11DSDesc, sizeof(d3d11DSDesc) );

		d3d11DSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3d11DSDesc.Texture2D.MipSlice = 0;
		d3d11DSDesc.Format = d3d11Texture2DDesc.Format;

		if ( SUCCEEDED ( m_pd3d11Device->CreateDepthStencilView ( m_pd3d11PrimeDSBuffer, &d3d11DSDesc, &m_pd3d11PrimeDSView ) ) )
		{
			return true;
		}
	}

	return false;
}

bool CDirect3D11::SetRenderTargetAndDepthBuffer ( )
{
	if ( m_pd3d11PrimeRTView && m_pd3d11PrimeDSView )
	{
		m_pd3d11DeviceContext->OMSetRenderTargets ( 1, &m_pd3d11PrimeRTView, m_pd3d11PrimeDSView );
		return true;
	}
	else
	{
		return false;
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


CDirect3D11::~CDirect3D11 ( )
{
	ShutDownRenderer ();
}