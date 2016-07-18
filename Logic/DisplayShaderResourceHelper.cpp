#include "stdafx.h"
#include "DisplayShaderResourceHelper.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "UtilWindowInfo.h"

#include "../Engine/ConVar.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/IMesh.h"
#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/IRenderer.h"

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

void CDisplayShaderResourceHelper::Render( IRenderer& renderer )
{
	if ( ShouldDraw( ) )
	{
		// 스냅샷으로 만들어지는 텍스쳐의 경우 로드시에는 없기때문에 렌더때 텍스쳐가 없으면 세팅을 시도합니다.
		if ( GetModel( ) && GetModel( )->GetTexture() == nullptr )
		{
			GetModel( )->SetTexture( renderer.GetShaderResourceFromFile( m_textureName ) );
		}

		CGameObject::Render( renderer );
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

bool CDisplayShaderResourceHelper::LoadModelMesh( IRenderer& renderer )
{
	IMeshBuilder* pMeshBuilder = renderer.GetMeshBuilder( );

	if ( GetModel( ) != nullptr || pMeshBuilder == nullptr )
	{
		return false;
	}

	float wndWidth = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetWidth( ));
	float wndHeight = static_cast<float>(CUtilWindowInfo::GetInstance( ).GetHeight( ));

	float halfWidth = m_width / wndWidth;
	float halfHeight = m_height / wndHeight;

	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( -halfWidth, -halfHeight, 1.f ), D3DXVECTOR2( 0.f, 1.f ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( -halfWidth, halfHeight, 1.f ), D3DXVECTOR2( 0.f, 0.0f ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( halfWidth, -halfHeight, 1.f ), D3DXVECTOR2( 1.f, 1.0f ) ) );
	pMeshBuilder->Append( MeshVertex( D3DXVECTOR3( halfWidth, halfHeight, 1.f ), D3DXVECTOR2( 1.f, 0.f ) ) );

	pMeshBuilder->AppendIndex( 0 );
	pMeshBuilder->AppendIndex( 1 );
	pMeshBuilder->AppendIndex( 2 );
	pMeshBuilder->AppendIndex( 3 );

	pMeshBuilder->AppendTextureName( m_textureName );

	SetModelMeshName( GetName( ) );
	SetModel( pMeshBuilder->Build( renderer, GetMeshName( ), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ) );

	return GetModel( ) ? true : false;
}

CDisplayShaderResourceHelper::CDisplayShaderResourceHelper( ) :
	m_width( 1.0f ),
	m_height( 1.0f )
{}