#include "stdafx.h"
#include "Scene/ScreenBlurManager.h"

#include "Core/GameLogic.h"
//#include "Model/IModelBuilder.h"
//#include "Render/IRenderer.h"
//#include "Render/IRenderResourceManager.h"
#include "SizedTypes.h"

namespace logic
{
	void ScreenBlurManager::OnDeviceRestore( CGameLogic& gameLogic )
	{
		CreateDeviceDependentResource( gameLogic );
	}

	bool ScreenBlurManager::Init( CGameLogic& gameLogic )
	{
		return CreateDeviceDependentResource( gameLogic );
	}

	//void ScreenBlurManager::Process( CGameLogic& gameLogic, RE_HANDLE destSRV, RE_HANDLE destRT ) const
	//{
	//	IRenderer& renderer = gameLogic.GetRenderer( );
	//	RE_HANDLE default[] = { RE_HANDLE::InValidHandle( ) };
	//
	//	// Set RenderTarget
	//	renderer.BindRenderTargets( &m_blurRt, 1, default[0] );
	//
	//	// Set Gaussian_X Shader State
	//	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 1, &destSRV );
	//	m_pScreenRect->SetMaterial( m_blurMaterial[0] );
	//	m_pScreenRect->Draw( gameLogic );
	//
	//	// Set RenderTarget
	//	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 1, default );
	//	renderer.BindRenderTargets( &destRT, 1, default[0] );
	//
	//	// Set Gaussian_Y Shader State
	//	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 1, &m_blurSrv );
	//	m_pScreenRect->SetMaterial( m_blurMaterial[0] );
	//	m_pScreenRect->Draw( gameLogic );
	//}

	void ScreenBlurManager::AppSizeChanged( CGameLogic& gameLogic )
	{
		CreateAppSizeDependentResource( gameLogic );
	}

	bool ScreenBlurManager::CreateDeviceDependentResource( CGameLogic& gameLogic )
	{
		//IRenderer& renderer = gameLogic.GetRenderer( );

		//constexpr char* BLUR_MATERIAL_NAME[] = { "mat_gaussian_blur_x", "mat_gaussian_blur_y" };

		//for ( uint32 i = 0; i < 2; ++i )
		//{
		//	m_blurMaterial[i] = renderer.SearchMaterial( BLUR_MATERIAL_NAME[i] );

		//	if ( m_blurMaterial[i] == INVALID_MATERIAL )
		//	{
		//		return false;
		//	}
		//}

		//// Create Screen Rect Mesh
		//IModelBuilder& meshBuilder = gameLogic.GetModelManager( ).GetModelBuilder();
		//meshBuilder.Clear( );

		//meshBuilder.Append( MeshVertex( CXMFLOAT3( -1.f, -1.f, 1.f ), CXMFLOAT2( 0.f, 1.f ) ) );
		//meshBuilder.Append( MeshVertex( CXMFLOAT3( -1.f, 1.f, 1.f ), CXMFLOAT2( 0.f, 0.f ) ) );
		//meshBuilder.Append( MeshVertex( CXMFLOAT3( 1.f, -1.f, 1.f ), CXMFLOAT2( 1.f, 1.f ) ) );
		//meshBuilder.Append( MeshVertex( CXMFLOAT3( 1.f, 1.f, 1.f ), CXMFLOAT2( 1.f, 0.f ) ) );

		//meshBuilder.AppendIndex( 0 );
		//meshBuilder.AppendIndex( 1 );
		//meshBuilder.AppendIndex( 2 );
		//meshBuilder.AppendIndex( 3 );

		//m_pScreenRect = meshBuilder.Build( renderer, "ScreenRect", D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		//if ( m_pScreenRect == nullptr )
		//{
		//	return false;
		//}

		//if ( CreateAppSizeDependentResource( gameLogic ) == false )
		//{
		//	return false;
		//}

		return true;
	}

	bool ScreenBlurManager::CreateAppSizeDependentResource( CGameLogic& gameLogic )
	{
		//IRenderer& renderer = gameLogic.GetRenderer( );

		//std::string blurTempTextureName( "BlurTempTexture" );
		//IResourceManager& resourceMgr = renderer.GetResourceManager( );

		//m_blurTexture = resourceMgr.CreateTexture2D( blurTempTextureName, blurTempTextureName );
		//if ( m_blurTexture == RE_HANDLE::InValidHandle( ) )
		//{
		//	return false;
		//}

		//m_blurRt = resourceMgr.CreateRenderTarget( m_blurTexture, blurTempTextureName );
		//if ( m_blurRt == RE_HANDLE::InValidHandle( ) )
		//{
		//	return false;
		//}

		//m_blurSrv = resourceMgr.CreateTextureShaderResource( m_blurTexture, blurTempTextureName );
		//if ( m_blurSrv == RE_HANDLE::InValidHandle( ) )
		//{
		//	return false;
		//}

		return true;
	}
}
