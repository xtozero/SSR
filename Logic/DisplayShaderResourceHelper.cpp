#include "stdafx.h"
#include "DisplayShaderResourceHelper.h"

#include "GameLogic.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "Model/IMesh.h"
#include "Model/IModelBuilder.h"
#include "UtilWindowInfo.h"

#include "../Engine/ConVar.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"

namespace
{
	ConVar( r_debugTexture, "0", "show texture for debug\ndepthStencilViewer : depth\nrenderTargetViewer : backBuffer" );
}

DECLARE_GAME_OBJECT( displaySRVHelper, CDisplayShaderResourceHelper );

void CDisplayShaderResourceHelper::SetPosition( const CXMFLOAT3& pos )
{
	float wndWidth = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetWidth( ) );
	float wndHeight = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetHeight( ) );

	CXMFLOAT3 projPos( ( ( pos.x / wndWidth) - 0.5f ) * 2.f, ( ( pos.y / wndHeight ) - 0.5f ) * -2.f, 0.f );
	CGameObject::SetPosition( projPos );
}

void CDisplayShaderResourceHelper::Render( CGameLogic& gameLogic )
{
	if ( ShouldDraw( ) )
	{
		// 스냅샷으로 만들어지는 텍스쳐의 경우 로드시에는 없기때문에 렌더때 텍스쳐가 없으면 세팅을 시도합니다.
		if ( GetModel( ) && GetModel( )->GetTexture() == nullptr )
		{
			GetModel( )->SetTexture( gameLogic.GetRenderer().GetShaderResourceFromFile( m_textureName ) );
		}

		CGameObject::Render( gameLogic );
	}
}

void CDisplayShaderResourceHelper::Think( )
{
}

void CDisplayShaderResourceHelper::LoadPropertyFromScript( const KeyValue& keyValue )
{
	if ( const KeyValue* pTexName = keyValue.Find( _T( "TextureName" ) ) )
	{
		m_textureName = pTexName->GetValue( );
	}
	
	if ( const KeyValue* pTexWidth = keyValue.Find( _T( "Width" ) ) )
	{
		m_width = pTexWidth->GetValue<float>( );
	}
	
	if ( const KeyValue* pTexHeight = keyValue.Find( _T( "Height" ) ) )
	{
		m_height = pTexHeight->GetValue<float>( );
	}

	CGameObject::LoadPropertyFromScript( keyValue );
}

bool CDisplayShaderResourceHelper::ShouldDraw( ) const
{
	return r_debugTexture.GetString( ) == GetName( );
}

bool CDisplayShaderResourceHelper::LoadModelMesh( CGameLogic& gameLogic )
{
	IModelBuilder& meshBuilder = gameLogic.GetModelManager( ).GetModelBuilder( );

	if ( GetModel( ) != nullptr )
	{
		return false;
	}

	float wndWidth = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetWidth( ));
	float wndHeight = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetHeight( ));

	float halfWidth = m_width / wndWidth;
	float halfHeight = m_height / wndHeight;

	meshBuilder.Append( MeshVertex( CXMFLOAT3( -halfWidth, -halfHeight, 1.f ), CXMFLOAT2( 0.f, 1.f ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( -halfWidth, halfHeight, 1.f ), CXMFLOAT2( 0.f, 0.0f ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( halfWidth, -halfHeight, 1.f ), CXMFLOAT2( 1.f, 1.0f ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( halfWidth, halfHeight, 1.f ), CXMFLOAT2( 1.f, 0.f ) ) );

	meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 1 );
	meshBuilder.AppendIndex( 2 );
	meshBuilder.AppendIndex( 3 );

	meshBuilder.AppendTextureName( m_textureName );

	SetModelMeshName( GetName( ) );
	SetModel( meshBuilder.Build( gameLogic.GetRenderer(), GetMeshName( ), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ) );

	return GetModel( ) ? true : false;
}

CDisplayShaderResourceHelper::CDisplayShaderResourceHelper( ) :
	m_width( 1.0f ),
	m_height( 1.0f )
{}