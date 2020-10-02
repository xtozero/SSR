#include "stdafx.h"
#include "GameObject/StaticMeshGameObject.h"

#include "Components/Component.h"
#include "Components/StaticMeshComponent.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

DECLARE_GAME_OBJECT( static_mesh, StaticMeshGameObject );

void StaticMeshGameObject::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	CGameObject::LoadProperty( gameLogic, json );

	if ( const JSON::Value* pModel = json.Find( "Model" ) )
	{
		LoadModelMesh( gameLogic, pModel->AsString( ) );
	}
}

StaticMeshGameObject::StaticMeshGameObject( )
{
	m_rootComponent = CreateComponent<StaticMeshComponent>( *this );
}

bool StaticMeshGameObject::LoadModelMesh( CGameLogic& gameLogic, std::string assetPath )
{
	if ( assetPath.length( ) > 0 )
	{
		CModelManager& modelManager = gameLogic.GetModelManager( );
		// ¸ðµ¨·Îµå
		CModelManager::LoadCompletionCallback onMeshLoadComplete;
		onMeshLoadComplete.BindMemberFunction( this, &StaticMeshGameObject::OnModelLoadFinished );

		ModelLoaderSharedHandle handle = modelManager.RequestAsyncLoad( assetPath.c_str( ), onMeshLoadComplete );

		return handle->IsLoadingInProgress( ) || handle->IsLoadComplete( );
	}

	return false;
}

void StaticMeshGameObject::OnModelLoadFinished( void* model )
{
	StaticMeshComponent* staticMeshComponent = GetComponent<StaticMeshComponent>( );

	if ( staticMeshComponent )
	{
		staticMeshComponent->SetStaticMesh( static_cast<StaticMesh*>( model ) );
	}
}
