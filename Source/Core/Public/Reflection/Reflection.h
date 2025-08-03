#pragma once

#include "MethodMacros.h"
#include "PropertyMacros.h"
#include "TypeInfoMacros.h"

template <typename To, typename From>
To* Cast( From* src )
{
	return src && src->GetTypeInfo().template IsChildOf<To>() ? reinterpret_cast<To*>( src ) : nullptr;
}