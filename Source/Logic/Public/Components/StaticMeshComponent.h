#pragma once

#include "PrimitiveComponent.h"

#include <memory>

namespace rendercore
{
	class StaticMesh;
	class RenderOption;
}

class StaticMeshComponent : public PrimitiveComponent
{
	GENERATE_CLASS_TYPE_INFO( StaticMeshComponent )

public:
	using PrimitiveComponent::PrimitiveComponent;

	virtual void LoadProperty( const JSON::Value& json ) override;

	virtual BoxSphereBounds CalcBounds( const Matrix& transform ) override;

	virtual rendercore::PrimitiveProxy* CreateProxy() const override;
	virtual BodySetup* GetBodySetup() override;

	void SetStaticMesh( const std::shared_ptr<rendercore::StaticMesh>& pStaticMesh );
	std::shared_ptr<rendercore::StaticMesh> GetStaticMesh() const { return m_pStaticMesh; }

	void SetRenderOption( const std::shared_ptr<rendercore::RenderOption>& pRenderOption );
	std::shared_ptr<rendercore::RenderOption> GetRenderOption() const { return m_pRenderOption; }

private:
	bool LoadModelMesh( const std::string& assetPath );
	bool LoadRenderOption( const std::string& assetPath );

	void OnModelLoadFinished( const std::shared_ptr<void>& model );
	void OnRenderOptionLoadFinished( const std::shared_ptr<void>& renderOption );

	std::shared_ptr<rendercore::StaticMesh> m_pStaticMesh = nullptr;
	std::shared_ptr<rendercore::RenderOption> m_pRenderOption = nullptr;
};
