#pragma once

#include "AssetFactory.h"
#include "BaseMesh.h"
#include "IAsyncLoadableAsset.h"
#include "Physics/BoxSphereBounds.h"
#include "SizedTypes.h"

#include <vector>

namespace rendercore
{
	struct MeshDescription;
	class Material;
	class MaterialResource;
	class StaticMeshLODResource;
	class StaticMeshRenderData;

	struct StaticMeshMaterial final
	{
		explicit StaticMeshMaterial( const std::shared_ptr<Material>& mateiral ) : m_mateiral( mateiral ) {}
		StaticMeshMaterial() = default;

		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, StaticMeshMaterial& m );

		std::shared_ptr<Material> m_mateiral;
	};

	class StaticMesh final : public AsyncLoadableAsset, BaseMesh
	{
		GENERATE_CLASS_TYPE_INFO( StaticMesh );
		DECLARE_ASSET( RENDERCORE, StaticMesh );

	public:
		RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

		RENDERCORE_DLL void BuildMeshFromMeshDescriptions( const std::vector<MeshDescription>& meshDescriptions );
		void BuildMeshFromMeshDescription( const MeshDescription& meshDescription, StaticMeshLODResource& lodResource );

		RENDERCORE_DLL MaterialResource* GetMaterialResource( size_t idx ) const;
		RENDERCORE_DLL void AddMaterial( const std::shared_ptr<Material>& mateiral );

		RENDERCORE_DLL StaticMesh() = default;
		RENDERCORE_DLL virtual ~StaticMesh() override;
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
				m_bounds = std::move( other.m_bounds );
			}

			return *this;
		}

		StaticMeshRenderData* RenderData() const
		{
			return m_renderData;
		}

		BoxSphereBounds& Bounds()
		{
			return m_bounds;
		}

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

	private:
		StaticMeshRenderData* m_renderData = nullptr;

		PROPERTY( materials )
		std::vector<StaticMeshMaterial> m_materials;

		PROPERTY( bounds )
		BoxSphereBounds m_bounds;
	};
}
