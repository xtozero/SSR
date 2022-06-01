#pragma once

#include "EnumClassFlags.h"

enum class GAMEOBJECT_PROPERTY
{
	NONE = 0,
	REFLECTABLE_OBJECT = 1 << 0,
	REMOVE_ME = 1 << 1,
};
ENUM_CLASS_FLAGS( GAMEOBJECT_PROPERTY );