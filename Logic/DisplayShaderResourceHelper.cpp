#include "stdafx.h"
#include "DisplayShaderResourceHelper.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "UtilWindowInfo.h"

#include "../Engine/ConVar.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/Direct3D11.h"

extern IMeshBuilder* g_meshBuilder;
extern IRenderer* gRenderer;

namespace
{
	ConVar( r_debugTexture, "0", "show texture for debug\ndepthStencilViewer : depth\nrenderTargetViewer : backBuffer" );
}

DECLARE_GAME_OBJECT( displaySRVHelper, CDisplayShaderResourceHelper );

void CDisplayShaderResourceHelper::SetPosition( const D3DXVECTOR3& pos )
{
	float wndWidth = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetWidth( ) );
	float wndHeight = static_cast<float>( CUtilWindowInfo::GetInstance( ).GetHeight( ) );

	D3DXVECTOR3 projPos( ( ( pos.x / wndWidth) - 0.5f ) * 2.f, ( ( pos.y / wndHeight ) - 0.5f ) * -2.f, 0.f );
	CGameObject::SetPosition( projPos );
}

void CDisplayShaderResourceHelper::Render( )
{
	if ( ShouldDraw( ) )
	{
		// 스냅샷으로 만들어지는 텍스쳐의 경우 로드시에는 없기때문에 렌더때 텍스쳐가 없으면 세팅을 시도합니다.
		if ( GetModel( ) && GetModel( )->GetTexture() == nullptr )
		{
			GetModel( )->SetTexture( gRenderer->GetShaderResourceFromFile( m_textureName ).get() );
		}

		CGameObject::Render( );
	}
}

void CDisplayShaderResourceHelper::Think( )
{
}

bool CDisplayShaderResourceHelper::LoadPropertyFromScript( const CKeyValueIterator& pKeyValue )
{
	if ( pKeyValue->GetKey( ) == String( _T( "TextureName" ) ) )
	{
		m_textureName = pKeyValue->GetString( );

		return true;
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Width" ) ) )
	{
		m_width = pKeyValue->GetFloat( );

		return true;
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Height" ) ) )
	{
		m_height = pKeyValue->GetFloat( );

		return true;
	}

	return CGameObject::LoadPropertyFromScript( pKeyValue );
}

bool CDisplayShaderResourceHelper::ShouldDraw( ) const
{
	return r_debugTexture.GetString( ) == GetName( );
}

bool CDisplayShaderResourceHelper::LoadModelMesh( )
{
	if ( GetModel( ) != nullptr || g_meshBuilder == nullptr )
	{
		return false;
	}

	float wndWidth = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetWidth( ));
	float wndHeight = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetHeight( ));

	float halfWidth = m_width / wndWidth;
	float halfHeight = m_height / wndHeight;

	g_meshBuilder->Clear( );

	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( -halfWidth, -halfHeight, 1.f ), D3DXVECTOR2( 0.f, 1.f ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( -halfWidth, halfHeight, 1.f ), D3DXVECTOR2( 0.f, 0.0f ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( halfWidth, -halfHeight, 1.f ), D3DXVECTOR2( 1.f, 1.0f ) ) );
	g_meshBuilder->Append( MeshVertex( D3DXVECTOR3( halfWidth, halfHeight, 1.f ), D3DXVECTOR2( 1.f, 0.f ) ) );

	g_meshBuilder->AppendIndex( 0 );
	g_meshBuilder->AppendIndex( 1 );
	g_meshBuilder->AppendIndex( 2 );
	g_meshBuilder->AppendIndex( 3 );

	g_meshBuilder->AppendTextureName( m_textureName );

	SetModelMeshName( GetName( ) );
	SetModel( g_meshBuilder->Build( GetMeshName( ), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ) );

	return GetModel( ) ? true : false;
}

CDisplayShaderResourceHelper::CDisplayShaderResourceHelper( ) :
	m_width( 1.0f ),
	m_height( 1.0f )
{}