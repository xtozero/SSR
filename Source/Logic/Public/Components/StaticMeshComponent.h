#pragma once

#include "PrimitiveComponent.h"
#include "Mesh/StaticMesh.h"

#include <memory>

namespace rendercore
{
	class Material;
	class RenderOption;
}

namespace logic
{
	class StaticMeshComponent : public PrimitiveComponent
	{
		GENERATE_CLASS_TYPE_INFO( StaticMeshComponent )

	public:
		using PrimitiveComponent::PrimitiveComponent;

		virtual void LoadProperty( const json::Value& json ) override;

		virtual BoxSphereBounds CalcBounds( const Matrix& transform ) override;

		virtual rendercore::PrimitiveProxy* CreateProxy() const override;
		virtual BodySetup* GetBodySetup() override;

		void SetStaticMesh( const std::shared_ptr<rendercore::StaticMesh>& pStaticMesh );
		std::shared_ptr<rendercore::StaticMesh> GetStaticMesh() const { return m_pStaticMesh; }

		void SetRenderOption( const std::shared_ptr<rendercore::RenderOption>& pRenderOption );
		std::shared_ptr<rendercore::RenderOption> GetRenderOption() const { return m_pRenderOption; }

		void SetMaterial( size_t index, const std::shared_ptr<rendercore::Material>& pMaterial );
		std::shared_ptr<rendercore::Material> GetMaterial( size_t index ) const
		{
			if ( ( index < m_materials.size() ) && ( m_materials[index] != nullptr ) )
			{
				return m_materials[index];
			}

			if ( GetStaticMesh() )
			{
				return GetStaticMesh()->GetMaterial( index );
			}

			return nullptr;
		}

		uint32 NumMaterials() const
		{
			uint32 numMaterials = static_cast<uint32>( m_materials.size() );

			if ( GetStaticMesh() )
			{
				numMaterials = std::max( numMaterials, GetStaticMesh()->NumMaterials() );
			}

			return numMaterials;
		}

	private:
		void LoadModelMesh( const std::string& assetPath );
		void LoadRenderOption( const std::string& assetPath );
		void LoadMaterial( size_t index, const std::string& assetPath );

		void OnModelLoadFinished( const std::shared_ptr<void>& model );
		void OnRenderOptionLoadFinished( const std::shared_ptr<void>& renderOption );
		void OnMaterialLoadFinished( size_t index, const std::shared_ptr<void>& material );

		std::shared_ptr<rendercore::StaticMesh> m_pStaticMesh = nullptr;
		std::shared_ptr<rendercore::RenderOption> m_pRenderOption = nullptr;

		std::vector<std::shared_ptr<rendercore::Material>> m_materials;
	};
}
