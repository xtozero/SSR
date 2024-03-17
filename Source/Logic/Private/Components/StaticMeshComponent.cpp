#include "Components/StaticMeshComponent.h"

#include "AssetLoader.h"
#include "Json/Json.hpp"
#include "Mesh/StaticMesh.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include <cassert>

namespace logic
{
	void StaticMeshComponent::LoadProperty( const json::Value& json )
	{
		Super::LoadProperty( json );

		if ( const json::Value* pModel = json.Find( "Model" ) )
		{
			LoadModelMesh( pModel->AsString() );
		}

		if ( const json::Value* pRenderOption = json.Find( "RenderOption" ) )
		{
			LoadRenderOption( pRenderOption->AsString() );
		}
	}

	BoxSphereBounds StaticMeshComponent::CalcBounds( const Matrix& transform )
	{
		if ( m_pStaticMesh )
		{
			BoxSphereBounds bounds = m_pStaticMesh->Bounds().TransformBy( transform );
			return bounds;
		}

		Vector position( transform._41, transform._42, transform._43 );
		return BoxSphereBounds( position, Vector( 0, 0, 0 ), 0.f );
	}

	rendercore::PrimitiveProxy* StaticMeshComponent::CreateProxy() const
	{
		if ( m_pStaticMesh == nullptr
			|| m_pStaticMesh->RenderData() == nullptr
			|| m_pRenderOption == nullptr )
		{
			return nullptr;
		}

		return new rendercore::StaticMeshPrimitiveProxy( *this );
	}

	BodySetup* StaticMeshComponent::GetBodySetup()
	{
		return nullptr;
	}

	void StaticMeshComponent::SetStaticMesh( const std::shared_ptr<rendercore::StaticMesh>& pStaticMesh )
	{
		assert( pStaticMesh != nullptr );
		m_pStaticMesh = pStaticMesh;

		MarkRenderStateDirty();

		UpdateBounds();
	}

	void StaticMeshComponent::SetRenderOption( const std::shared_ptr<rendercore::RenderOption>& pRenderOption )
	{
		assert( pRenderOption != nullptr );
		m_pRenderOption = pRenderOption;

		MarkRenderStateDirty();
	}

	bool StaticMeshComponent::LoadModelMesh( const std::string& assetPath )
	{
		if ( assetPath.length() > 0 )
		{
			// ¸ðµ¨·Îµå
			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindMemberFunction( this, &StaticMeshComponent::OnModelLoadFinished );

			AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>()->RequestAsyncLoad( assetPath, onLoadComplete );

			return handle->IsLoadingInProgress() || handle->IsLoadComplete();
		}

		return false;
	}

	bool StaticMeshComponent::LoadRenderOption( const std::string& assetPath )
	{
		if ( assetPath.length() > 0 )
		{
			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindMemberFunction( this, &StaticMeshComponent::OnRenderOptionLoadFinished );

			AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>()->RequestAsyncLoad( assetPath, onLoadComplete );

			return handle->IsLoadingInProgress() || handle->IsLoadComplete();
		}

		return false;
	}

	void StaticMeshComponent::OnModelLoadFinished( const std::shared_ptr<void>& model )
	{
		SetStaticMesh( std::static_pointer_cast<rendercore::StaticMesh>( model ) );
	}

	void StaticMeshComponent::OnRenderOptionLoadFinished( const std::shared_ptr<void>& renderOption )
	{
		SetRenderOption( std::static_pointer_cast<rendercore::RenderOption>( renderOption ) );
	}
}
