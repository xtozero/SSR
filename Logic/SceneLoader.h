#pragma once

#include "common.h"
#include "GameObject.h"
#include "..\Engine\KeyValueReader.h"

#include <memory.h>
#include <vector>

class CSceneLoader
{
public:
	std::shared_ptr<KeyValueGroup> LoadSceneFromFile( const String& fileName, std::vector<std::shared_ptr<CGameObject>>& objectList );

private:
	void SetSceneObjectProperty( std::shared_ptr<KeyValueGroup> keyValue, std::vector<std::shared_ptr<CGameObject>>& objectList );
};

