#include "stdafx.h"
#include "common.h"

#include "BaseMesh.h"

#include "D3D11IndexBuffer.h"
#include "D3D11PixelShader.h"
#include "D3D11VertexBuffer.h"
#include "D3D11VertexShader.h"

#include "DebugMesh.h"
#include "DepthStencil.h"
#include "DepthStencilStateFactory.h"

#include "Direct3D11.h"

#include "ISampler.h"
#include "IShaderResource.h"

#include "MaterialCommon.h"
#include "MaterialSystem.h"
#include "MaterialLoader.h"
#include "MeshBuilder.h"

#include "RasterizerStateFactory.h"
#include "RenderTarget.h"
#include "SamplerStateFactory.h"
#include "SkyBoxMaterial.h"
#include "SnapShotManager.h"
#include "TextureMaterial.h"
#include "TutorialMaterial.h"
#include "WireFrame.h"

#include <array>
#include <tuple>
#include <string>

#include "ConstantBufferDefine.h"
#include "util_rendercore.h"

#include "../Engine/EnumStringMap.h"
#include "../shared/Util.h"

#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>

namespace
{
	IRenderer* CreateDirect3D11Renderer( )
	{
		static CDirect3D11 direct3D11;
		return &direct3D11;
	}

	IMeshBuilder* GetMeshBuilder( )
	{
		static CMeshBuilder meshBuilder;
		return &meshBuilder;
	}

	const int WORLD_MATRIX_ELEMENT_SIZE = 2;
};

bool CDirect3D11::InitializeRenderer( HWND hWind, UINT nWndWidth, UINT nWndHeight )
{
	ON_FAIL_RETURN( CreateD3D11Device( hWind, nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN( CreatePrimeRenderTargetVIew( ) );
	ON_FAIL_RETURN( CreatePrimeDepthBuffer( nWndWidth, nWndHeight ) );
	ON_FAIL_RETURN( SetRenderTargetDepthStencilView( ) );

	m_pView = std::make_unique<RenderView>( );
	ON_FAIL_RETURN( m_pView->initialize( m_pd3d11Device.Get( ) ) );

	m_pDepthStencilFactory = CreateDepthStencailStateFactory( );
	ON_FAIL_RETURN( m_pDepthStencilFactory );
	m_pDepthStencilFactory->LoadDesc( );

	m_pRasterizerFactory = CreateRasterizerStateFactory( );
	ON_FAIL_RETURN( m_pRasterizerFactory );
	m_pRasterizerFactory->LoadDesc( );

	m_pSamplerFactory = CreateSamplerStateFactory( );
	ON_FAIL_RETURN( m_pSamplerFactory );
	m_pSamplerFactory->LoadDesc( );

	ON_FAIL_RETURN( InitializeShaders( ) );
	ON_FAIL_RETURN( InitializeMaterial( ) );
	ON_FAIL_RETURN( m_meshLoader.Initialize( ) );
	
	ON_FAIL_RETURN( m_textureManager.LoadTexture( m_pd3d11Device.Get() ) );

	ON_FAIL_RETURN( m_renderEffect.Initialize( m_pd3d11Device.Get(), m_pd3d11DeviceContext.Get(), &m_textureManager, &m_shaderResourceManager, &m_renderTargetManager, &m_snapshotManager ) );

	m_pWorldMatrixBuffer = CreateConstantBuffer( _T("WorldMatrix"), sizeof( D3DXMATRIX ), WORLD_MATRIX_ELEMENT_SIZE, NULL );
	MaterialSystem::GetInstance( )->RegisterConstantBuffer( MAT_CONSTANT_BUFFER::SURFACE, CreateConstantBuffer( _T( "Surface" ), sizeof( SurfaceTrait ), 1, NULL ) );

	if ( m_pWorldMatrixBuffer == nullptr )
	{
		return false;
	}

	return true;
}

void CDirect3D11::ShutDownRenderer( )
{
	m_pd3d11DefaultRT = nullptr;
	m_pd3d11DefaultDS = nullptr;

	m_shaderList.clear( );
	m_bufferList.clear( );
	m_constantBufferList.clear( );

	m_pView = std::move( nullptr );

	m_pWorldMatrixBuffer = nullptr;

	m_pDepthStencilFactory = std::move( nullptr );
	m_pRasterizerFactory = std::move( nullptr );
	m_pSamplerFactory = std::move( nullptr );
	m_pMaterialLoader = std::move( nullptr );

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

	if ( m_pd3d11DefaultRT )
	{
		m_pd3d11DefaultRT->Clear( m_pd3d11DeviceContext.Get(), clearColor );
	}
}

void CDirect3D11::ClearDepthStencilView( )
{
	if ( m_pd3d11DefaultDS )
	{
		m_pd3d11DefaultDS->Clear( m_pd3d11DeviceContext.Get( ), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
	}
}

void CDirect3D11::SceneBegin( )
{
	ClearDepthStencilView( );
	ClearRenderTargetView( );
	if ( m_pView )
	{
		m_pView->UpdataView( m_pd3d11DeviceContext.Get( ) );
	}

	m_renderEffect.SceneBegin( );
}

void CDirect3D11::SceneEnd( )
{
	m_pdxgiSwapChain->Present( 0, 0 );

	m_snapshotManager.TakeSnapshot2D( m_pd3d11Device.Get( ), m_pd3d11DeviceContext.Get( ), _T( "DefaultDepthStencil" ), _T( "DebugDepthStencil" ) );
	m_snapshotManager.TakeSnapshot2D( m_pd3d11Device.Get( ), m_pd3d11DeviceContext.Get( ), _T( "DefaultRenderTarget" ), _T( "DebugRenderTarget" ) );

	m_renderEffect.SceneEnd( );
}

std::shared_ptr<IShader> CDirect3D11::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile )
{
	std::shared_ptr<D3D11VertexShader> vs = std::make_shared<D3D11VertexShader>( );

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

	return nullptr;
}

std::shared_ptr<IShader> CDirect3D11::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	std::shared_ptr<D3D11PixelShader> ps = std::make_shared<D3D11PixelShader>( );
	if ( ps->CreateShader( m_pd3d11Device.Get( ), pFilePath, pProfile ) )
	{
		m_shaderList.emplace( UTIL::GetFileName( pFilePath ), ps );
		return ps;
	}

	return nullptr;
}

std::shared_ptr<IBuffer> CDirect3D11::CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData )
{
	std::shared_ptr<D3D11VertexBuffer> vb = std::make_shared<D3D11VertexBuffer>( );
	if ( vb->CreateBuffer( m_pd3d11Device.Get( ), stride, numOfElement, srcData ) )
	{
		m_bufferList.emplace_back( vb );
		return vb;
	}

	return nullptr;
}

std::shared_ptr<IBuffer> CDirect3D11::CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData )
{
	std::shared_ptr<D3D11IndexBuffer> ib = std::make_shared<D3D11IndexBuffer>( );
	if ( ib->CreateBuffer( m_pd3d11Device.Get( ), stride, numOfElement, srcData ) )
	{
		m_bufferList.emplace_back( ib );
		return ib;
	}

	return nullptr;
}

std::shared_ptr<IBuffer> CDirect3D11::CreateConstantBuffer( const String& bufferName, const UINT stride, const UINT numOfElement, const void* srcData )
{
	std::shared_ptr<D3D11ConstantBuffer> cb = std::make_shared<D3D11ConstantBuffer>( );
	if ( cb->CreateBuffer( m_pd3d11Device.Get( ), stride, numOfElement, srcData ) )
	{
		m_constantBufferList.emplace( bufferName, cb );
		return cb;
	}

	return nullptr;
}

void* CDirect3D11::MapConstantBuffer( const String& bufferName )
{
	auto found = m_constantBufferList.find( bufferName );

	if ( found != m_constantBufferList.end( ) )
	{
		return found->second->LockBuffer( m_pd3d11DeviceContext.Get( ) );
	}

	return nullptr;
}

void CDirect3D11::UnMapConstantBuffer( const String& bufferName )
{
	auto found = m_constantBufferList.find( bufferName );

	if ( found != m_constantBufferList.end( ) )
	{
		found->second->UnLockBuffer( m_pd3d11DeviceContext.Get() );
	}
}

void CDirect3D11::SetConstantBuffer( const String & bufferName, const UINT slot, const SHADER_TYPE type )
{
	auto found = m_constantBufferList.find( bufferName );

	if ( found != m_constantBufferList.end( ) )
	{
		switch ( type )
		{
		case SHADER_TYPE::VS:
			found->second->SetVSBuffer( m_pd3d11DeviceContext.Get( ), slot );
			break;
		case SHADER_TYPE::PS:
			found->second->SetPSBuffer( m_pd3d11DeviceContext.Get( ), slot );
			break;
		}
	}
}

std::shared_ptr<IShader> CDirect3D11::SearchShaderByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	auto found = m_shaderList.find( pName );

	if ( found != m_shaderList.end( ) )
	{
		return found->second;
	}

	return nullptr;
}

bool CDirect3D11::InitializeMaterial( )
{
	REGISTER_MATERIAL( TutorialMaterial, tutorial );
	REGISTER_MATERIAL( WireFrame, wireframe );
	REGISTER_MATERIAL( TextureMaterial, texture );
	REGISTER_MATERIAL( SkyBoxMaterial, skybox );

	m_pMaterialLoader = CreateMaterialLoader( );

	if ( m_pMaterialLoader )
	{
		return m_pMaterialLoader->LoadMaterials( );
	}

	return false;
}

void CDirect3D11::RegisterEnumString( )
{
	//Register enum string
	//DXGI_FORMAT
	REGISTER_ENUM_STRING( DXGI_FORMAT_UNKNOWN );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32A32_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32A32_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32A32_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32A32_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32B32_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16B16A16_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16B16A16_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16B16A16_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16B16A16_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16B16A16_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16B16A16_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G32_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32G8X24_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_D32_FLOAT_S8X24_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_X32_TYPELESS_G8X24_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R10G10B10A2_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R10G10B10A2_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R10G10B10A2_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R11G11B10_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8B8A8_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8B8A8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8B8A8_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8B8A8_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8B8A8_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16G16_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_D32_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R32_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R24G8_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_D24_UNORM_S8_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_X24_TYPELESS_G8_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16_FLOAT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_D16_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R16_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8_UINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8_SINT );
	REGISTER_ENUM_STRING( DXGI_FORMAT_A8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R1_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R9G9B9E5_SHAREDEXP );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R8G8_B8G8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_G8R8_G8B8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC1_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC1_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC1_UNORM_SRGB );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC2_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC2_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC2_UNORM_SRGB );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC3_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC3_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC3_UNORM_SRGB );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC4_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC4_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC4_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC5_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC5_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC5_SNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B5G6R5_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B5G5R5A1_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B8G8R8A8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B8G8R8X8_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B8G8R8A8_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B8G8R8A8_UNORM_SRGB );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B8G8R8X8_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_B8G8R8X8_UNORM_SRGB );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC6H_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC6H_UF16 );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC6H_SF16 );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC7_TYPELESS );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC7_UNORM );
	REGISTER_ENUM_STRING( DXGI_FORMAT_BC7_UNORM_SRGB );
	REGISTER_ENUM_STRING( DXGI_FORMAT_FORCE_UINT );

	//USAGE
	REGISTER_ENUM_STRING( D3D11_USAGE_DEFAULT );
	REGISTER_ENUM_STRING( D3D11_USAGE_IMMUTABLE );
	REGISTER_ENUM_STRING( D3D11_USAGE_DYNAMIC );
	REGISTER_ENUM_STRING( D3D11_USAGE_STAGING );

	//BIND
	REGISTER_ENUM_STRING( D3D11_BIND_VERTEX_BUFFER );
	REGISTER_ENUM_STRING( D3D11_BIND_INDEX_BUFFER );
	REGISTER_ENUM_STRING( D3D11_BIND_CONSTANT_BUFFER );
	REGISTER_ENUM_STRING( D3D11_BIND_SHADER_RESOURCE );
	REGISTER_ENUM_STRING( D3D11_BIND_STREAM_OUTPUT );
	REGISTER_ENUM_STRING( D3D11_BIND_RENDER_TARGET );
	REGISTER_ENUM_STRING( D3D11_BIND_DEPTH_STENCIL );
	REGISTER_ENUM_STRING( D3D11_BIND_UNORDERED_ACCESS );

	//CPU
	REGISTER_ENUM_STRING( D3D11_CPU_ACCESS_WRITE );
	REGISTER_ENUM_STRING( D3D11_CPU_ACCESS_READ );

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
}

std::shared_ptr<IRenderTarget> CDirect3D11::TranslateRenderTargetViewFlag( const RENDERTARGET_FLAG rtFlag ) const
{
	switch ( rtFlag )
	{
	case RENDERTARGET_FLAG::NONE:
		return nullptr;
		break;
	case RENDERTARGET_FLAG::DEFALUT:
		return m_pd3d11DefaultRT;
		break;
	default:
		return nullptr;
		break;
	}
}

std::shared_ptr<IDepthStencil>  CDirect3D11::TranslateDepthStencilViewFlag( const DEPTHSTENCIL_FLAG dsFlag ) const
{
	switch ( dsFlag )
	{
	case DEPTHSTENCIL_FLAG::NONE:
		return nullptr;
		break;
	case DEPTHSTENCIL_FLAG::DEFALUT:
		return m_pd3d11DefaultDS;
		break;
	default:
		return nullptr;
		break;
	}
}

std::shared_ptr<IMaterial> CDirect3D11::GetMaterialPtr( const TCHAR* pMaterialName )
{
	return MaterialSystem::GetInstance( )->SearchMaterialByName( pMaterialName );
}

std::shared_ptr<IMesh> CDirect3D11::GetModelPtr( const TCHAR* pModelName )
{
	if ( m_meshLoader.LoadMeshFromFile( pModelName, &m_surfaceManager ) )
	{
		return m_meshLoader.GetMesh( pModelName );
	}

	return nullptr;
}

void CDirect3D11::SetModelPtr( const String& modelName, const std::shared_ptr<IMesh>& pModel )
{
	if ( pModel )
	{
		m_meshLoader.RegisterMesh( modelName, pModel );
	}
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

void CDirect3D11::UpdateWorldMatrix( const D3DXMATRIX& worldMatrix, const D3DXMATRIX& invWorldMatrix )
{
	if ( m_pWorldMatrixBuffer == nullptr )
	{
		DebugWarning( "m_pWorldMatrixBuffer is nullptr\n" );
		return;
	}

	D3DXMATRIX* pWorld = static_cast<D3DXMATRIX*>( m_pWorldMatrixBuffer->LockBuffer( m_pd3d11DeviceContext.Get( ) ) );

	if ( pWorld )
	{
		std::array<D3DXMATRIX, 2> transposWorld;

		D3DXMatrixTranspose( &transposWorld[0], &worldMatrix );
		D3DXMatrixTranspose( &transposWorld[1], &invWorldMatrix );

		CopyMemory( pWorld, transposWorld.data(), sizeof( D3DXMATRIX ) * WORLD_MATRIX_ELEMENT_SIZE );
	}

	m_pWorldMatrixBuffer->UnLockBuffer( m_pd3d11DeviceContext.Get( ) );
	m_pWorldMatrixBuffer->SetVSBuffer( m_pd3d11DeviceContext.Get( ), static_cast<int>( VS_CONSTANT_BUFFER::WORLD ) );
}

Microsoft::WRL::ComPtr<ID3D11RasterizerState> CDirect3D11::CreateRenderState( const String& stateName )
{
	if ( m_pRasterizerFactory == nullptr )
	{
		DebugWarning( "RasterizerFactory is nullptr" );
		return nullptr;
	}

	return m_pRasterizerFactory->GetRasterizerState( m_pd3d11Device.Get( ), stateName );
}

std::shared_ptr<IShaderResource> CDirect3D11::GetShaderResourceFromFile( const String& fileName )
{
	m_shaderResourceManager.LoadShaderResourceFromFile( m_pd3d11Device.Get( ), fileName );
	return m_shaderResourceManager.FindShaderResource( fileName );
}

std::shared_ptr<ISampler> CDirect3D11::CreateSamplerState( const String& stateName )
{
	if ( m_pSamplerFactory == nullptr )
	{
		DebugWarning( "SamplerFactory is nullptr" );
		return nullptr;
	}

	return CreateSampler( m_pSamplerFactory->GetSamplerState( m_pd3d11Device.Get( ), stateName ) );
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CDirect3D11::CreateDepthStencilState( const String& stateName )
{
	if ( m_pDepthStencilFactory == nullptr )
	{
		DebugWarning( "DepthStencilFactory is nullptr" );
		return nullptr;
	}

	return m_pDepthStencilFactory->GetDepthStencilState( m_pd3d11Device.Get( ), stateName );
}

bool CDirect3D11::SetRenderTargetDepthStencilView( RENDERTARGET_FLAG rtFlag, DEPTHSTENCIL_FLAG dsFlag )
{
	m_renderTargetManager.SetRenderTarget( m_pd3d11DeviceContext.Get(), TranslateRenderTargetViewFlag( rtFlag ), TranslateDepthStencilViewFlag( dsFlag ) );
	return true;
}

void CDirect3D11::ResetResource( const std::shared_ptr<IMesh>& pMesh, const SHADER_TYPE type )
{
	if ( m_pd3d11DeviceContext && pMesh )
	{
		pMesh->ResetResource( m_pd3d11DeviceContext.Get( ), type );
	}
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
		Microsoft::WRL::ComPtr<ID3D11Resource> renderTargetResource;

		if ( SUCCEEDED( pd3d11BackBuffer.Get()->QueryInterface( IID_ID3D11Resource, &renderTargetResource ) ) )
		{
			ON_FAIL_RETURN( m_textureManager.RegisterTexture2D( _T( "DefaultRenderTarget" ), renderTargetResource ) );
		}

		if ( m_renderTargetManager.CreateRenderTarget( m_pd3d11Device.Get( ), pd3d11BackBuffer.Get( ), nullptr, _T( "DefaultRenderTarget" ) ) )
		{
			m_pd3d11DefaultRT = m_renderTargetManager.FindRenderTarget( _T( "DefaultRenderTarget" ) );
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
	d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	d3d11Texture2DDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	d3d11Texture2DDesc.Height = nWndHeight;
	d3d11Texture2DDesc.Width = nWndWidth;
	d3d11Texture2DDesc.MipLevels = 1;
	d3d11Texture2DDesc.SampleDesc.Count = 1;
	d3d11Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;

	const ITexture* depthStencilTexture = m_textureManager.CreateTexture2D( m_pd3d11Device.Get( ), d3d11Texture2DDesc, _T( "DefaultDepthStencil" ) );

	if ( depthStencilTexture )
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DSDesc;
		::ZeroMemory( &d3d11DSDesc, sizeof( d3d11DSDesc ) );

		d3d11DSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3d11DSDesc.Texture2D.MipSlice = 0;
		d3d11DSDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		ON_FAIL_RETURN( m_renderTargetManager.CreateDepthStencil( m_pd3d11Device.Get( ), depthStencilTexture, &d3d11DSDesc, _T( "DefaultDepthStencil" ) ) );
		m_pd3d11DefaultDS = m_renderTargetManager.FindDepthStencil( _T( "DefaultDepthStencil" ) );

		return true;
	}

	return false;
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

bool CDirect3D11::InitializeShaders( )
{
	return m_shaderLoader.LoadShadersFromScript( this );
}

CDirect3D11::CDirect3D11( ) : m_pd3d11Device( nullptr ),
m_pd3d11DeviceContext( nullptr ),
m_pdxgiSwapChain( nullptr ),
m_pd3d11DefaultRT( nullptr ),
m_pd3d11DefaultDS( nullptr ),
m_pWorldMatrixBuffer( nullptr ),
m_snapshotManager( &m_textureManager, &m_shaderResourceManager ),
m_pDepthStencilFactory( nullptr ),
m_pRasterizerFactory( nullptr ),
m_pSamplerFactory( nullptr ),
m_pMaterialLoader( nullptr )
{
	RegisterEnumString( );
}


CDirect3D11::~CDirect3D11( )
{
	ShutDownRenderer( );
}