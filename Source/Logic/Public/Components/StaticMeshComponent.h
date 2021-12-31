#pragma once

#include "PrimitiveComponent.h"

#include <memory>

class StaticMesh;
class RenderOption;

class StaticMeshComponent : public PrimitiveComponent
{
public:
	using PrimitiveComponent::PrimitiveComponent;

	virtual BoxSphereBounds CalcBounds( const Matrix& transform ) override;

	virtual PrimitiveProxy* CreateProxy( ) const override;

	void SetStaticMesh( const std::shared_ptr<StaticMesh>& pStaticMesh );
	std::shared_ptr<StaticMesh> GetStaticMesh( ) const { return m_pStaticMesh; }

	void SetRenderOption( const std::shared_ptr<RenderOption>& pRenderOption );
	std::shared_ptr<RenderOption> GetRenderOption( ) const { return m_pRenderOption; }

private:
	std::shared_ptr<StaticMesh> m_pStaticMesh = nullptr;
	std::shared_ptr<RenderOption> m_pRenderOption = nullptr;
};