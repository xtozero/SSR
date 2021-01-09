#pragma once

#include "AssetLoader/AssetFactory.h"
#include "BaseMesh.h"
#include "Core/IAsyncLoadableAsset.h"
#include "Material/Material.h"

#include <vector>

struct MeshDescription;
class Material;
class ObjMeshAssetProcessor;
class StaticMeshLODResource;
class StaticMeshRenderData;

struct StaticMeshMaterial
{
	StaticMeshMaterial( Material* mateiral ) : m_mateiral( mateiral ) {}

	Material* m_mateiral;
};

class StaticMesh : public AsyncLoadableAsset, BaseMesh
{
	DECLARE_ASSET( StaticMesh );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	LOGIC_DLL void BuildMeshFromMeshDescriptions( const std::vector<MeshDescription>& meshDescriptions );
	void BuildMeshFromMeshDescription( const MeshDescription& meshDescription, StaticMeshLODResource& lodResource );

	LOGIC_DLL void AddMaterial( Material* mateiral );

	LOGIC_DLL StaticMesh( );
	StaticMesh( MeshDescription&& meshDescription, std::vector<Material>&& materials );
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

private:
	friend ObjMeshAssetProcessor;

	StaticMeshRenderData* m_renderData = nullptr;
	std::vector<StaticMeshMaterial> m_materials;
};
