#pragma once

#include "Property.h"

#define PROPERTY( Name )	\
	inline static struct RegistPropertyExecutor_##Name	\
	{	\
		RegistPropertyExecutor_##Name()	\
		{	\
			static PropertyRegister<ThisType, decltype(m_##Name), decltype(&ThisType::m_##Name), &ThisType::m_##Name> property_register_##Name{ #Name, ThisType::StaticTypeInfo() }; \
		}	\
		\
	} regist_##Name;