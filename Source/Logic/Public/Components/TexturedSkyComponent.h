#pragma once

#include "Component.h"

#include <memory>

class StaticMesh;
class Material;
class TexturedSkyProxy;

class TexturedSkyComponent : public Component
{
	GENERATE_CLASS_TYPE_INFO( TexturedSkyComponent )

public:
	using Component::Component;

	virtual void LoadProperty( const JSON::Value& json ) override;

	virtual TexturedSkyProxy* CreateProxy() const;

	void SetStaticMesh( const std::shared_ptr<StaticMesh>& pStaticMesh );
	std::shared_ptr<StaticMesh> GetStaticMesh() const { return m_pStaticMesh; }

	void SetMaterial( const std::shared_ptr<Material>& pMaterial );
	std::shared_ptr<Material> GetMaterial() const { return m_pMaterial; }

	TexturedSkyProxy* m_texturedSkyProxy = nullptr;

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

private:
	void OnMaterialLoadFinished( const std::shared_ptr<void>& material );

	std::shared_ptr<StaticMesh> m_pStaticMesh = nullptr;
	std::shared_ptr<Material> m_pMaterial = nullptr;
};