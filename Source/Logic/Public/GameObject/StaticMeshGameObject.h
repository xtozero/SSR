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
	bool LoadModelMesh( CGameLogic& gameLogic, const std::string& assetPath );
	bool LoadRenderOption( CGameLogic& gameLogic, const std::string& assetPath );
	void OnModelLoadFinished( void* model );

	void OnRenderOptionLoadFinished( void* renderOption );
};