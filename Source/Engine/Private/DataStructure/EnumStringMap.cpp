#include "stdafx.h"
#include "DataStructure/EnumStringMap.h"

#include <map>

class CEnumStringMap : public IEnumStringMap
{
public:
	virtual void RegisterEnumString( const String& enumString, const int enumValue ) override;
	virtual int GetEnum( const String& enumName, const int defaultValue ) const override;

private:
	std::map<String, int> m_enumString;
};


void CEnumStringMap::RegisterEnumString( const String& enumString, const int enumValue )
{
	m_enumString.emplace( enumString, enumValue );
}

int CEnumStringMap::GetEnum( const String& enumName, const int defaultValue ) const
{
	auto found = m_enumString.find( enumName );

	if ( found != m_enumString.end( ) )
	{
		return found->second;
	}

	return defaultValue;
}

void* GetEnumStringMap( )
{
	static CEnumStringMap gEnumStringMap;
	return &gEnumStringMap;
}