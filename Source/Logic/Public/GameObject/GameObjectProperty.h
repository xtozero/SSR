#pragma once

#include "EnumClassFlags.h"

namespace logic
{
	enum class GameobjectProperty : uint8
	{
		None = 0,
		ReflectableObject = 1 << 0,
		RemoveMe = 1 << 1,
	};
	ENUM_CLASS_FLAGS( GameobjectProperty );
}