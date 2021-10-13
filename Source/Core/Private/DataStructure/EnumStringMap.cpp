#include "EnumStringMap.h"

#include <map>

class CEnumStringMap : public IEnumStringMap
{
public:
	virtual void RegisterEnumString( const std::string& enumString, const int32 enumValue ) override;
	virtual int32 GetEnum( const std::string& enumName, const int32 defaultValue ) const override;

private:
	std::map<std::string, int32> m_enumString;
};


void CEnumStringMap::RegisterEnumString( const std::string& enumString, const int32 enumValue )
{
	m_enumString.emplace( enumString, enumValue );
}

int32 CEnumStringMap::GetEnum( const std::string& enumName, const int32 defaultValue ) const
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