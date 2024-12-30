#pragma once

#include "Reflection.h"

class Archive;

class Serializable
{
	GENERATE_CLASS_TYPE_INFO( Serializable );

public:
	virtual void Serialize( Archive& ar )
	{
		for ( const auto& property : GetTypeInfo().GetProperties() )
		{
			property->Serialize( this, ar );
		}
	}

	virtual ~Serializable() = default;
};