#pragma once

#include "GameObject.h"

class TexturedSkyComponent;

class CSkyBox : public CGameObject
{
public:
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;

	virtual bool IgnorePicking( ) const override { return true; }

public:
	CSkyBox( );
	~CSkyBox( ) = default;

private:
	void OnMaterialLoadFinished( const std::shared_ptr<void>& material );

	TexturedSkyComponent* m_pTexturedSkyComponent = nullptr;
};
