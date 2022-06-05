#pragma once

#include "PrimitiveComponent.h"

#include <memory>

class StaticMesh;
class RenderOption;

class StaticMeshComponent : public PrimitiveComponent
{
	GENERATE_CLASS_TYPE_INFO( StaticMeshComponent )

public:
	using PrimitiveComponent::PrimitiveComponent;

	virtual void LoadProperty( const JSON::Value& json ) override;

	virtual BoxSphereBounds CalcBounds( const Matrix& transform ) override;

	virtual PrimitiveProxy* CreateProxy() const override;
	virtual BodySetup* GetBodySetup() override;

	void SetStaticMesh( const std::shared_ptr<StaticMesh>& pStaticMesh );
	std::shared_ptr<StaticMesh> GetStaticMesh() const { return m_pStaticMesh; }

	void SetRenderOption( const std::shared_ptr<RenderOption>& pRenderOption );
	std::shared_ptr<RenderOption> GetRenderOption() const { return m_pRenderOption; }

private:
	bool LoadModelMesh( const std::string& assetPath );
	bool LoadRenderOption( const std::string& assetPath );

	void OnModelLoadFinished( const std::shared_ptr<void>& model );
	void OnRenderOptionLoadFinished( const std::shared_ptr<void>& renderOption );

	std::shared_ptr<StaticMesh> m_pStaticMesh = nullptr;
	std::shared_ptr<RenderOption> m_pRenderOption = nullptr;
};