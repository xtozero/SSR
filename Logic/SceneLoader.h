#pragma once

#include "common.h"

#include <memory.h>
#include <vector>

class CGameObject;
class CGameLogic;
class KeyValue;

class CSceneLoader
{
public:
	std::unique_ptr<KeyValue> LoadSceneFromFile( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& objectList, const String& fileName );

private:
	void SetSceneObjectProperty( CGameLogic& gameLogic, KeyValue* keyValue, std::vector<std::unique_ptr<CGameObject>>& objectList );
};

