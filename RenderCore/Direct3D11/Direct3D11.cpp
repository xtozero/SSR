#include "stdafx.h"

#include "../common.h"
#include "../CommonRenderer/IMaterial.h"
#include "../CommonRenderer/IRenderer.h"
#include "../CommonRenderer/IRenderResource.h"
#include "../CommonRenderer/IRenderState.h"

#include "D3D11BlendStateFactory.h"
#include "D3D11Buffer.h"
#include "D3D11ComputeShader.h"
#include "D3D11DepthStencil.h"
#include "D3D11DepthStencilStateFactory.h"
#include "D3D11PixelShader.h"
#include "D3D11RasterizerStateFactory.h"
#include "D3D11RenderStateManager.h"
#include "D3D11RenderTarget.h"
#include "D3D11Resource.h"
#include "D3D11ResourceManager.h"
#include "D3D11SamplerStateFactory.h"
#include "D3D11ShaderUtil.h"
#include "D3D11VertexShader.h"

#include "../MaterialSystem.h"
#include "../MaterialLoader.h"

#include "../RenderCoreDllFunc.h"
#include "../RenderEffect.h"
#include "../RenderOutputManager.h"

#include "../ShaderListScriptLoader.h"
#include "../SkyBoxMaterial.h"
#include "../TextureMaterial.h"
#include "../TutorialMaterial.h"
#include "../WireFrame.h"

#include "../util_rendercore.h"

#include "../../Engine/EnumStringMap.h"
#include "../../shared/Util.h"

#include <array>
#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>
#include <map>
#include <tuple>
#include <string>
#include <vector>

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
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;
	virtual void ShutDownRenderer( ) override;
	virtual void SceneBegin( ) override;
	virtual void ForwardRenderEnd( ) override;
	virtual void SceneEnd( ) override;

	virtual IShader* CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual IShader* CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual IComputeShader* CreateComputeShader( const TCHAR* pFilePath, const char* pProfile ) override;

	virtual IBuffer* CreateBuffer( const BUFFER_TRAIT& trait ) override;

	virtual IShader* FindGraphicsShaderByName( const TCHAR* pName ) override;
	virtual IComputeShader* FindComputeShaderByName( const TCHAR* pName ) override;

	virtual IMaterial* GetMaterialPtr( const TCHAR* pMaterialName ) override;

	virtual void SetViewports( std::vector<Viewport>& viewports ) override;
	virtual void SetScissorRects( std::vector<RECT>& rects ) override;

	virtual IRenderState* CreateRenderState( const String& stateName ) override;

	virtual IRenderResource* GetShaderResourceFromFile( const String& fileName ) override;
	virtual IRenderState* CreateSamplerState( const String& stateName ) override;

	virtual IRenderState* CreateDepthStencilState( const String& stateName ) override;

	virtual IRenderState* CreateBlendState( const String& stateName ) override;

	virtual void TakeSnapshot2D( const String& sourceTextureName, const String& destTextureName ) override;

	virtual void PSSetShaderResource( UINT startSlot, IRenderResource* shaderResourceOrNull ) override;

	virtual void ClearRendertarget( IRenderResource& renderTarget, const float( &clearColor )[4] ) override;
	virtual void ClearDepthStencil( IRenderResource& depthStencil, float depthColor, UINT8 stencilColor ) override;

	virtual void SetRenderTarget( IRenderResource* pRenderTarget, IRenderResource* pDepthStencil ) override;
	virtual void SetRenderTarget( RenderTargetBinder& binder, IRenderResource* pDepthStencil ) override;

	virtual void Draw( UINT primitive, UINT vertexCount, UINT vertexOffset = 0 ) override;
	virtual void DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) override;
	virtual void DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) override;
	virtual void DrawAuto( UINT primitive ) override;

	virtual IResourceManager& GetResourceManager( ) override { return *m_resourceManager.get( ); }

	CDirect3D11( );
	virtual ~CDirect3D11( );
private:
	bool CreateDeviceDependentResource( HWND hWnd, UINT nWndWidth, UINT nWndHeight );
	bool CreateDeviceIndependentResource( );
	void ReportLiveDevice( );
	bool InitializeShaders( );
	bool InitializeMaterial( );

	Microsoft::WRL::ComPtr<ID3D11Device>					m_pd3d11Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>				m_pd3d11DeviceContext;
	Microsoft::WRL::ComPtr<IDXGIFactory1>					m_pdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain>					m_pdxgiSwapChain;

	std::map<String, std::unique_ptr<IShader>>				m_graphicsShaderList;
	std::map<String, std::unique_ptr<IComputeShader>>		m_computeShaderList;
	std::vector<std::unique_ptr<IBuffer>>					m_bufferList;

	std::unique_ptr<CD3D11ResourceManager>					m_resourceManager;
	CRenderOutputManager									m_renderOutput;

	CShaderListScriptLoader									m_shaderLoader;

	std::unique_ptr<CD3D11RenderStateManager>				m_pRenderStateManager;
	CD3D11DepthStencilStateFactory							m_DepthStencilFactory;
	CD3D11RasterizerStateFactory							m_RasterizerFactory;
	CD3D11SamplerStateFactory								m_SamplerFactory;
	CD3D11BlendStateFactory									m_BlendFactory;
	CMaterialLoader											m_MaterialLoader;

	CRenderEffect											m_renderEffect;
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

	m_pRenderStateManager = std::make_unique<CD3D11RenderStateManager>( *m_pd3d11DeviceContext.Get( ) );

	m_DepthStencilFactory.LoadDesc( );
	m_RasterizerFactory.LoadDesc( );
	m_SamplerFactory.LoadDesc( );
	m_BlendFactory.LoadDesc( );

	if ( !InitializeShaders( ) )
	{
		return false;
	}

	if ( !InitializeMaterial( ) )
	{
		return false;
	}
	
	m_resourceManager = std::make_unique<CD3D11ResourceManager>( *m_pd3d11Device.Get( ), *m_pd3d11DeviceContext.Get( ) );
	m_resourceManager->AppSizeChanged( nWndWidth, nWndHeight );
	if ( !m_resourceManager->Bootup( ) )
	{
		return false;
	}

	if ( !m_renderOutput.Initialize( *m_resourceManager, *m_pdxgiSwapChain.Get( ) ) )
	{
		return false;
	}

	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pd3d11BackBuffer = nullptr;
		m_pdxgiSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( pd3d11BackBuffer.GetAddressOf( ) ) );
	}

	return true;
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

	if ( m_resourceManager == nullptr )
	{
		__debugbreak( );
	}

	m_resourceManager->AppSizeChanged( nWndWidth, nWndHeight );

	HRESULT hr = m_pdxgiSwapChain->ResizeBuffers( 1, nWndWidth, nWndHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
	if ( FAILED( hr ) )
	{
		__debugbreak( );
	}

	m_renderOutput.AppSizeChanged( *m_resourceManager, *m_pdxgiSwapChain.Get() );
}

void CDirect3D11::ShutDownRenderer( )
{
	m_graphicsShaderList.clear( );
	m_bufferList.clear( );

#ifdef _DEBUG
	ReportLiveDevice( );
#endif
}

void CDirect3D11::SceneBegin( )
{
	m_renderOutput.SetRenderTargetDepthStencilView( *this );
	m_renderOutput.ClearDepthStencil( *this );
	m_renderOutput.ClearRenderTargets( *this, rtClearColor( 1.0f, 1.0f, 1.0f, 1.0f ) );

	m_renderEffect.SceneBegin( *this );
}

void CDirect3D11::ForwardRenderEnd( )
{
	TakeSnapshot2D( _T( "DepthGBuffer" ), _T( "DuplicateDepthGBuffer" ) );
	TakeSnapshot2D( _T( "DefaultRenderTarget" ), _T( "DuplicateFrameBuffer" ) );
}

void CDirect3D11::SceneEnd( )
{
	m_pdxgiSwapChain->Present( 0, 0 );

	TakeSnapshot2D( _T( "DefaultDepthStencil" ), _T( "DebugDepthStencil" ) );
	TakeSnapshot2D( _T( "ShadowMap" ), _T( "DebugShadowMap" ) );

	m_renderOutput.SceneEnd( *m_pd3d11DeviceContext.Get() );

	m_renderEffect.SceneEnd( *this );
}

IShader* CDirect3D11::CreateVertexShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	if ( IShader* found = FindGraphicsShaderByName( fileName.c_str() ) )
	{
		return found;
	}

	std::unique_ptr<D3D11VertexShader> vs = std::make_unique<D3D11VertexShader>( *m_pRenderStateManager );

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

	bool result = false;
	CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );
	if ( byteCode.GetBufferSize( ) > 0 )
	{
		result = vs->CreateShader( *m_pd3d11Device.Get( ), byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = vs->CreateShader( *m_pd3d11Device.Get( ), shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
	}

	if ( result )
	{
		D3D11VertexShader* ret = vs.get( );
		m_graphicsShaderList.emplace( fileName, std::move( vs ) );
		return ret;
	}

	return nullptr;
}

IShader* CDirect3D11::CreatePixelShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	if ( IShader* found = FindGraphicsShaderByName( fileName.c_str( ) ) )
	{
		return found;
	}

	std::unique_ptr<D3D11PixelShader> ps = std::make_unique<D3D11PixelShader>( *m_pRenderStateManager );
	bool result = false;
	CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );
	
	if ( byteCode.GetBufferSize( ) > 0 )
	{
		result = ps->CreateShader( *m_pd3d11Device.Get( ), byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = ps->CreateShader( *m_pd3d11Device.Get( ), shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
	}

	if ( result )
	{
		D3D11PixelShader* ret = ps.get( );
		m_graphicsShaderList.emplace( fileName, std::move( ps ) );
		return ret;
	}

	return nullptr;
}

IComputeShader* CDirect3D11::CreateComputeShader( const TCHAR* pFilePath, const char* pProfile )
{
	String fileName = UTIL::GetFileName( pFilePath );
	if ( IComputeShader* found = FindComputeShaderByName( fileName.c_str( ) ) )
	{
		return found;
	}

	std::unique_ptr<D3D11ComputeShader> cs = std::make_unique<D3D11ComputeShader>( *m_pRenderStateManager );
	bool result = false;
	CShaderByteCode byteCode = GetCompiledByteCode( pFilePath );

	if ( byteCode.GetBufferSize( ) > 0 )
	{
		result = cs->CreateShader( *m_pd3d11Device.Get( ), byteCode.GetBufferPointer( ), byteCode.GetBufferSize( ) );
	}
	else if ( Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = GetShaderBlob( pFilePath, pProfile ) )
	{
		result = cs->CreateShader( *m_pd3d11Device.Get( ), shaderBlob->GetBufferPointer( ), shaderBlob->GetBufferSize( ) );
	}

	if ( result )
	{
		D3D11ComputeShader* ret = cs.get( );
		m_computeShaderList.emplace( fileName, std::move( cs ) );
		return ret;
	}

	return nullptr;
}

IBuffer* CDirect3D11::CreateBuffer( const BUFFER_TRAIT& trait )
{
	std::unique_ptr<CD3D11Buffer> buffer = std::make_unique<CD3D11Buffer>( *m_pRenderStateManager );
	
	if ( buffer->Create( *m_pd3d11Device.Get( ), trait ) )
	{
		CD3D11Buffer* ret = buffer.get( );
		m_bufferList.emplace_back( std::move( buffer ) );
		return ret;
	}

	return nullptr;
}

IShader* CDirect3D11::FindGraphicsShaderByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	auto found = m_graphicsShaderList.find( pName );

	if ( found != m_graphicsShaderList.end( ) )
	{
		return found->second.get();
	}

	return nullptr;
}

IComputeShader * CDirect3D11::FindComputeShaderByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	auto found = m_computeShaderList.find( pName );

	if ( found != m_computeShaderList.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

bool CDirect3D11::InitializeMaterial( )
{
	REGISTER_MATERIAL( *this, TutorialMaterial, tutorial );
	REGISTER_MATERIAL( *this, WireFrame, wireframe );
	REGISTER_MATERIAL( *this, TextureMaterial, texture );
	REGISTER_MATERIAL( *this, SkyBoxMaterial, skybox );

	return m_MaterialLoader.LoadMaterials( *this );
}

IMaterial* CDirect3D11::GetMaterialPtr( const TCHAR* pMaterialName )
{
	return MaterialSystem::GetInstance( )->SearchMaterialByName( pMaterialName );
}

void CDirect3D11::SetViewports( std::vector<Viewport>& viewports )
{
	std::vector<D3D11_VIEWPORT> d3d11Viewports;

	for ( const Viewport& vp : viewports )
	{
		D3D11_VIEWPORT newVeiwport = { vp.m_x, vp.m_y, vp.m_width, vp.m_height, vp.m_near, vp.m_far };

		d3d11Viewports.push_back( newVeiwport );
	}

	m_pd3d11DeviceContext->RSSetViewports( d3d11Viewports.size( ), d3d11Viewports.data( ) );
}

void CDirect3D11::SetScissorRects( std::vector<RECT>& rects )
{
	m_pd3d11DeviceContext->RSSetScissorRects( rects.size( ), rects.data( ) );
}

IRenderState* CDirect3D11::CreateRenderState( const String& stateName )
{
	return m_RasterizerFactory.GetRasterizerState( *m_pd3d11Device.Get( ), *m_pRenderStateManager, stateName );
}

IRenderResource* CDirect3D11::GetShaderResourceFromFile( const String& fileName )
{
	m_resourceManager->LoadShaderResourceFromFile( fileName );
	return m_resourceManager->FindShaderResource( fileName );
}

IRenderState* CDirect3D11::CreateSamplerState( const String& stateName )
{
	return m_SamplerFactory.GetSamplerState( *m_pd3d11Device.Get( ), *m_pRenderStateManager, stateName );
}

IRenderState* CDirect3D11::CreateDepthStencilState( const String& stateName )
{
	return m_DepthStencilFactory.GetDepthStencilState( *m_pd3d11Device.Get( ), *m_pRenderStateManager, stateName );
}

IRenderState* CDirect3D11::CreateBlendState( const String& stateName )
{
	return m_BlendFactory.GetBlendState( *m_pd3d11Device.Get( ), *m_pRenderStateManager, stateName );
}

void CDirect3D11::TakeSnapshot2D( const String& sourceTextureName, const String& destTextureName )
{
	m_resourceManager->TakeSnapshot( sourceTextureName, destTextureName );
}

void CDirect3D11::PSSetShaderResource( UINT startSlot, IRenderResource* shaderResourceOrNull )
{
	ID3D11ShaderResourceView* srv = static_cast<ID3D11ShaderResourceView*>( shaderResourceOrNull ? shaderResourceOrNull->Get( ) : nullptr );
	m_pRenderStateManager->SetPsShaderResource( startSlot, 1, &srv );
}

void CDirect3D11::ClearRendertarget( IRenderResource& renderTarget, const float (&clearColor)[4] )
{
	m_pd3d11DeviceContext->ClearRenderTargetView( static_cast<ID3D11RenderTargetView*>( renderTarget.Get( ) ), clearColor );
}

void CDirect3D11::ClearDepthStencil( IRenderResource& depthStencil, float depthColor, UINT8 stencilColor )
{
	m_pd3d11DeviceContext->ClearDepthStencilView( static_cast<ID3D11DepthStencilView*>( depthStencil.Get( ) ), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
}

void CDirect3D11::SetRenderTarget( IRenderResource* pRenderTarget, IRenderResource* pDepthStencil )
{
	ID3D11RenderTargetView* rtv = pRenderTarget ? static_cast<ID3D11RenderTargetView*>( pRenderTarget->Get( ) ) : nullptr;
	ID3D11DepthStencilView* dsv = pDepthStencil ? static_cast<ID3D11DepthStencilView*>( pDepthStencil->Get( ) ) : nullptr;

	m_pd3d11DeviceContext->OMSetRenderTargets( 1, &rtv, dsv );
}

void CDirect3D11::SetRenderTarget( RenderTargetBinder& binder, IRenderResource* pDepthStencil )
{
	ID3D11DepthStencilView* dsv = pDepthStencil ? static_cast<ID3D11DepthStencilView*>( pDepthStencil->Get( ) ) : nullptr;

	m_pd3d11DeviceContext->OMSetRenderTargets( binder.Count( ), binder.Get( ), dsv );
}

void CDirect3D11::Draw( UINT primitive, UINT vertexCount, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pRenderStateManager->SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->Draw( vertexCount, vertexOffset );
}

void CDirect3D11::DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset, UINT vertexOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pRenderStateManager->SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawIndexed( indexCount, indexOffset, vertexOffset );
}

void CDirect3D11::DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pRenderStateManager->SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawInstanced( vertexCount, instanceCount, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset, UINT vertexOffset, UINT instanceOffset )
{
	D3D_PRIMITIVE_TOPOLOGY d3d11Primitive = ConvertPrimToD3D11Prim( primitive );
	m_pRenderStateManager->SetPrimitiveTopology( d3d11Primitive );
	m_pd3d11DeviceContext->DrawIndexedInstanced( indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset );
}

void CDirect3D11::DrawAuto( UINT primitive )
{
	m_pd3d11DeviceContext->DrawAuto( );
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
			DXGI_SWAP_CHAIN_DESC dxgiSwapchainDesc = {};

			dxgiSwapchainDesc.BufferCount = 1;
			dxgiSwapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dxgiSwapchainDesc.BufferDesc.Height = nWndHeight;
			dxgiSwapchainDesc.BufferDesc.Width = nWndWidth;
			dxgiSwapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
			dxgiSwapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
			dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			dxgiSwapchainDesc.OutputWindow = hWnd;
			dxgiSwapchainDesc.SampleDesc.Count = 1;
			dxgiSwapchainDesc.SampleDesc.Quality = 0;
			dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			dxgiSwapchainDesc.Windowed = true;
			dxgiSwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			hr = m_pdxgiFactory->CreateSwapChain( m_pd3d11Device.Get( ), &dxgiSwapchainDesc, m_pdxgiSwapChain.GetAddressOf( ) );
			if ( FAILED( hr ) )
			{
				return false;
			}

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

bool CDirect3D11::InitializeShaders( )
{
	return m_shaderLoader.LoadShadersFromScript( *this );
}

IRenderer* CreateDirect3D11Renderer( )
{
	return new CDirect3D11( );
}