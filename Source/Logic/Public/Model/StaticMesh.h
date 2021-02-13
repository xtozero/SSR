#pragma once

#include "AssetLoader/AssetFactory.h"
#include "AssetLoader/IAsyncLoadableAsset.h"
#include "BaseMesh.h"
#include "Material/Material.h"

#include <vector>

struct MeshDescription;
class Material;
class ObjMeshAssetProcessor;
class StaticMeshLODResource;
class StaticMeshRenderData;

struct StaticMeshMaterial
{
	explicit StaticMeshMaterial( const std::shared_ptr<Material>& mateiral ) : m_mateiral( mateiral ) {}
	StaticMeshMaterial( ) = default;

	std::shared_ptr<const Material> m_mateiral;
};

class StaticMesh : public AsyncLoadableAsset, BaseMesh
{
	DECLARE_ASSET( LOGIC, StaticMesh );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	LOGIC_DLL void BuildMeshFromMeshDescriptions( const std::vector<MeshDescription>& meshDescriptions );
	void BuildMeshFromMeshDescription( const MeshDescription& meshDescription, StaticMeshLODResource& lodResource );

	LOGIC_DLL void AddMaterial( const std::shared_ptr<Material>& mateiral );

	LOGIC_DLL StaticMesh( ) = default;
	LOGIC_DLL StaticMesh( const StaticMesh& ) = delete;
	LOGIC_DLL StaticMesh( StaticMesh&& other )
	{
		*this = std::move( other );
	}
	LOGIC_DLL StaticMesh& operator=( const StaticMesh& ) = delete;
	LOGIC_DLL StaticMesh& operator=( StaticMesh&& other )
	{
		if ( this != &other )
		{
			m_renderData = other.m_renderData;
			other.m_renderData = nullptr;
			m_materials = std::move( other.m_materials );
		}

		return *this;
	}
	LOGIC_DLL ~StaticMesh( );

	StaticMeshRenderData* RenderData( ) const
	{
		return m_renderData;
	}

protected:
	LOGIC_DLL virtual void PostLoadImpl( ) override;

private:
	StaticMeshRenderData* m_renderData = nullptr;
	std::vector<StaticMeshMaterial> m_materials;
};
