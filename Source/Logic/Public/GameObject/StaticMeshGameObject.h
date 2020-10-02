#pragma once

#include "GameObject.h"

#include <string>

class CGameLogic;

class StaticMeshGameObject : public CGameObject
{
public:
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;

	StaticMeshGameObject( );

private:
	bool LoadModelMesh( CGameLogic& gameLogic, std::string assetPath );
	void StaticMeshGameObject::OnModelLoadFinished( void* model );
};