#pragma once

#include "Method.h"

#include <type_traits>

#define METHOD( Name )	\
	inline static struct RegistMethodExecutor_##Name	\
	{	\
		RegistMethodExecutor_##Name()	\
		{	\
			static MethodRegister<ThisType, decltype(&ThisType::Name), &ThisType::Name> method_register_##Name{ #Name, ThisType::StaticTypeInfo() };	\
		}	\
		\
	} regist_##Name;