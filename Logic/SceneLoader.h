#pragma once

#include "common.h"

#include <memory.h>
#include <vector>

class CGameObject;
class IRenderer;
class KeyValueGroup;

class CSceneLoader
{
public:
	std::unique_ptr<KeyValueGroup> LoadSceneFromFile( IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& objectList, const String& fileName );

private:
	void SetSceneObjectProperty( IRenderer& renderer, KeyValueGroup* keyValue, std::vector<std::unique_ptr<CGameObject>>& objectList );
};

