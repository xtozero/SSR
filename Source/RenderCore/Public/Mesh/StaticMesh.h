#pragma once

#include "AssetFactory.h"
#include "BaseMesh.h"
#include "IAsyncLoadableAsset.h"
#include "SizedTypes.h"

#include <vector>

struct MeshDescription;
class Material;
class MaterialResource;
class ObjMeshAssetProcessor;
class StaticMeshLODResource;
class StaticMeshRenderData;

struct StaticMeshMaterial
{
	explicit StaticMeshMaterial( const std::shared_ptr<Material>& mateiral ) : m_mateiral( mateiral ) {}
	StaticMeshMaterial( ) = default;

	std::shared_ptr<Material> m_mateiral;
};

class StaticMesh : public AsyncLoadableAsset, BaseMesh
{
	DECLARE_ASSET( RENDERCORE, StaticMesh );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	RENDERCORE_DLL void BuildMeshFromMeshDescriptions( const std::vector<MeshDescription>& meshDescriptions );
	void BuildMeshFromMeshDescription( const MeshDescription& meshDescription, StaticMeshLODResource& lodResource );

	RENDERCORE_DLL MaterialResource* GetMaterialResource( size_t idx ) const;
	RENDERCORE_DLL void AddMaterial( const std::shared_ptr<Material>& mateiral );

	RENDERCORE_DLL StaticMesh( ) = default;
	RENDERCORE_DLL ~StaticMesh( );
	RENDERCORE_DLL StaticMesh( const StaticMesh& ) = delete;
	RENDERCORE_DLL StaticMesh( StaticMesh&& other ) noexcept
	{
		*this = std::move( other );
	}
	RENDERCORE_DLL StaticMesh& operator=( const StaticMesh& ) = delete;
	RENDERCORE_DLL StaticMesh& operator=( StaticMesh&& other ) noexcept
	{
		if ( this != &other )
		{
			m_renderData = other.m_renderData;
			other.m_renderData = nullptr;
			m_materials = std::move( other.m_materials );
		}

		return *this;
	}

	StaticMeshRenderData* RenderData( ) const
	{
		return m_renderData;
	}

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	StaticMeshRenderData* m_renderData = nullptr;
	std::vector<StaticMeshMaterial> m_materials;
};
