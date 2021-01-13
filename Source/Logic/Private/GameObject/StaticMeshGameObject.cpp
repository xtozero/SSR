#include "stdafx.h"
#include "GameObject/StaticMeshGameObject.h"

#include "AssetLoader/AssetLoader.h"
#include "Components/Component.h"
#include "Components/StaticMeshComponent.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"
#include "Rendering/RenderOption.h"

DECLARE_GAME_OBJECT( static_mesh, StaticMeshGameObject );

void StaticMeshGameObject::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	CGameObject::LoadProperty( gameLogic, json );

	if ( const JSON::Value* pModel = json.Find( "Model" ) )
	{
		LoadModelMesh( gameLogic, pModel->AsString( ) );
	}

	if ( const JSON::Value* pRenderOption = json.Find( "RenderOption" ) )
	{
		LoadRenderOption( gameLogic, pRenderOption->AsString( ) );
	}
}

StaticMeshGameObject::StaticMeshGameObject( )
{
	m_rootComponent = CreateComponent<StaticMeshComponent>( *this );
}

bool StaticMeshGameObject::LoadModelMesh( CGameLogic& gameLogic, const std::string& assetPath )
{
	if ( assetPath.length( ) > 0 )
	{
		// ¸ðµ¨·Îµå
		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindMemberFunction( this, &StaticMeshGameObject::OnModelLoadFinished );

		AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>( )->RequestAsyncLoad( assetPath, onLoadComplete );

		return handle->IsLoadingInProgress( ) || handle->IsLoadComplete( );
	}

	return false;
}

bool StaticMeshGameObject::LoadRenderOption( CGameLogic& gameLogic, const std::string& assetPath )
{
	if ( assetPath.length( ) > 0 )
	{
		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindMemberFunction( this, &StaticMeshGameObject::OnRenderOptionLoadFinished );

		AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>( )->RequestAsyncLoad( assetPath, onLoadComplete );

		return handle->IsLoadingInProgress( ) || handle->IsLoadComplete( );
	}

	return false;
}

void StaticMeshGameObject::OnModelLoadFinished( const std::shared_ptr<void>& model )
{
	StaticMeshComponent* staticMeshComponent = GetComponent<StaticMeshComponent>( );

	if ( staticMeshComponent )
	{
		staticMeshComponent->SetStaticMesh( std::static_pointer_cast<StaticMesh>( model ) );
	}
}

void StaticMeshGameObject::OnRenderOptionLoadFinished( const std::shared_ptr<void>& renderOption )
{
	StaticMeshComponent* staticMeshComponent = GetComponent<StaticMeshComponent>( );

	if ( staticMeshComponent )
	{
		staticMeshComponent->SetRenderOption( std::static_pointer_cast<RenderOption>( renderOption ) );
	}
}
