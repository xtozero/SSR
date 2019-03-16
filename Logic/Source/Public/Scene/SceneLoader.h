#pragma once

#include "common.h"

#include <memory>
#include <vector>

class CGameObject;
class CGameLogic;
class KeyValue;

class CSceneLoader
{
public:
	std::unique_ptr<KeyValue> LoadSceneFromFile( CGameLogic& gameLogic, const String& fileName );

private:
	void SetSceneObjectProperty( CGameLogic& gameLogic, KeyValue* keyValue );
};

