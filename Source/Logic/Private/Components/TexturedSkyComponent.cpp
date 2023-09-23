#include "stdafx.h"
#include "Components/TexturedSkyComponent.h"

#include "AssetLoader.h"
#include "Json/Json.hpp"
#include "Proxies/TexturedSkyProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

namespace logic
{
	void TexturedSkyComponent::LoadProperty( const json::Value& json )
	{
		Super::LoadProperty( json );

		if ( const json::Value* pTexture = json.Find( "Material" ) )
		{
			const std::string& assetPath = pTexture->AsString();

			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindMemberFunction( this, &TexturedSkyComponent::OnMaterialLoadFinished );

			AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>()->RequestAsyncLoad( assetPath, onLoadComplete );

			assert( handle->IsLoadingInProgress() || handle->IsLoadComplete() );
		}
	}

	rendercore::TexturedSkyProxy* TexturedSkyComponent::CreateProxy() const
	{
		if ( m_pStaticMesh == nullptr ||
			m_pMaterial == nullptr )
		{
			return nullptr;
		}

		return new rendercore::TexturedSkyProxy( *this );
	}

	void TexturedSkyComponent::SetStaticMesh( const std::shared_ptr<rendercore::StaticMesh>& pStaticMesh )
	{
		assert( pStaticMesh != nullptr );
		m_pStaticMesh = pStaticMesh;

		MarkRenderStateDirty();
	}

	void TexturedSkyComponent::SetMaterial( const std::shared_ptr<rendercore::Material>& pMaterial )
	{
		assert( pMaterial != nullptr );
		m_pMaterial = pMaterial;

		MarkRenderStateDirty();
	}

	bool TexturedSkyComponent::ShouldCreateRenderState() const
	{
		return true;
	}

	void TexturedSkyComponent::CreateRenderState()
	{
		Component::CreateRenderState();
		m_pWorld->Scene()->AddTexturedSkyComponent( this );
	}

	void TexturedSkyComponent::RemoveRenderState()
	{
		Component::RemoveRenderState();
		m_pWorld->Scene()->RemoveTexturedSkyComponent( this );
	}

	void TexturedSkyComponent::OnMaterialLoadFinished( const std::shared_ptr<void>& material )
	{
		SetMaterial( std::static_pointer_cast<rendercore::Material>( material ) );
	}
}
