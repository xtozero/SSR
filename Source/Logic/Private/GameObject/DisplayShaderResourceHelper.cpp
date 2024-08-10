#if 0
#include "GameObject/DisplayShaderResourceHelper.h"

#include "ConsoleMessage/ConVar.h"
#include "Core/GameLogic.h"
#include "Core/UtilWindowInfo.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"
//#include "Model/IModelBuilder.h"
//#include "Render/IRenderer.h"

namespace
{
	ConVar( r_debugTexture, "0", "show texture for debug\ndepthStencilViewer : depth\nrenderTargetViewer : backBuffer" );
}

namespace logic
{
	DECLARE_GAME_OBJECT( displaySRVHelper, CDisplayShaderResourceHelper );

	//void CDisplayShaderResourceHelper::SetPosition( const CXMFLOAT3& pos )
	//{
	//	float wndWidth = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetWidth( ) );
	//	float wndHeight = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetHeight( ) );
	//
	//	CXMFLOAT3 projPos( ( ( pos.x / wndWidth) - 0.5f ) * 2.f, ( ( pos.y / wndHeight ) - 0.5f ) * -2.f, 0.f );
	//	CGameObject::SetPosition( projPos );
	//}

	//void CDisplayShaderResourceHelper::Render( CGameLogic& gameLogic )
	//{
		//if ( ShouldDraw( ) )
		//{
			//// 스냅샷으로 만들어지는 텍스쳐의 경우 로드시에는 없기때문에 렌더때 텍스쳐가 없으면 세팅을 시도합니다.
			//if ( GetModel( ) && GetModel( )->GetTexture() == RE_HANDLE::InValidHandle( ) )
			//{
			//	GetModel( )->SetTexture( gameLogic.GetRenderer().CreateShaderResourceFromFile( m_textureName ) );
			//}

			//CGameObject::Render( gameLogic );
		//}
	//}

	void CDisplayShaderResourceHelper::Think( float /*elapsedTime*/ )
	{
	}

	//void CDisplayShaderResourceHelper::LoadProperty( CGameLogic& gameLogic, const json::Value& json )
	//{
	//	CGameObject::LoadProperty( gameLogic, json );
	//
	//	if ( const json::Value* pTexName = json.Find( "TextureName" ) )
	//	{
	//		m_textureName = pTexName->AsString( );
	//	}
	//	
	//	if ( const json::Value* pTexWidth = json.Find( "Width" ) )
	//	{
	//		m_width = static_cast<float>( pTexWidth->AsReal( ) );
	//	}
	//	
	//	if ( const json::Value* pTexHeight = json.Find( "Height" ) )
	//	{
	//		m_height = static_cast<float>( pTexHeight->AsReal( ) );
	//	}
	//}

	//bool CDisplayShaderResourceHelper::ShouldDraw( ) const
	//{
	//	return r_debugTexture.GetString( ) == GetName( );
	//}

	//bool CDisplayShaderResourceHelper::LoadModelMesh( CGameLogic& gameLogic )
	//{
		//if ( GetModel( ) != nullptr )
		//{
		//	return false;
		//}

		//float wndWidth = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetWidth( ));
		//float wndHeight = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetHeight( ));

		//float halfWidth = m_width / wndWidth;
		//float halfHeight = m_height / wndHeight;

		//IModelBuilder& meshBuilder = gameLogic.GetModelManager( ).GetModelBuilder( );
		//meshBuilder.Clear( );

		//meshBuilder.Append( MeshVertex( CXMFLOAT3( -halfWidth, -halfHeight, 1.f ), CXMFLOAT2( 0.f, 1.f ) ) );
		//meshBuilder.Append( MeshVertex( CXMFLOAT3( -halfWidth, halfHeight, 1.f ), CXMFLOAT2( 0.f, 0.0f ) ) );
		//meshBuilder.Append( MeshVertex( CXMFLOAT3( halfWidth, -halfHeight, 1.f ), CXMFLOAT2( 1.f, 1.0f ) ) );
		//meshBuilder.Append( MeshVertex( CXMFLOAT3( halfWidth, halfHeight, 1.f ), CXMFLOAT2( 1.f, 0.f ) ) );

		//meshBuilder.AppendIndex( 0 );
		//meshBuilder.AppendIndex( 1 );
		//meshBuilder.AppendIndex( 2 );
		//meshBuilder.AppendIndex( 3 );

		//meshBuilder.AppendTextureName( m_textureName );

		//SetModelMeshName( GetName( ) );
		//SetModel( meshBuilder.Build( gameLogic.GetRenderer(), GetMeshName( ), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ) );

		//return GetModel( ) ? true : false;

	//	return true;
	//}

	CDisplayShaderResourceHelper::CDisplayShaderResourceHelper() :
		m_width( 1.0f ),
		m_height( 1.0f )
	{}
}
#endif
