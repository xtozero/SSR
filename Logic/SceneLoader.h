#pragma once

#include "common.h"

#include <memory.h>
#include <vector>

class CGameObject;
class KeyValueGroup;

class CSceneLoader
{
public:
	std::shared_ptr<KeyValueGroup> LoadSceneFromFile( const String& fileName, std::vector<std::shared_ptr<CGameObject>>& objectList );

private:
	void SetSceneObjectProperty( std::shared_ptr<KeyValueGroup> keyValue, std::vector<std::shared_ptr<CGameObject>>& objectList );
};

