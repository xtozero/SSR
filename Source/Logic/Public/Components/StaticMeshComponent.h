#pragma once

#include "PrimitiveComponent.h"

class StaticMesh;
class RenderOption;

class StaticMeshComponent : public PrimitiveComponent
{
public:
	using PrimitiveComponent::PrimitiveComponent;

	virtual PrimitiveProxy* CreateProxy( ) const override;

	void SetStaticMesh( StaticMesh* pStaticMesh );
	StaticMesh* GetStaticMesh( ) const { return m_pStaticMesh; }

	void SetRenderOption( const RenderOption* pRenderOption );
	const RenderOption* GetRenderOption( ) const { return m_pRenderOption; }

private:
	StaticMesh* m_pStaticMesh = nullptr;
	const RenderOption* m_pRenderOption = nullptr;
};