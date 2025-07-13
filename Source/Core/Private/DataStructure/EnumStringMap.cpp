#include "EnumStringMap.h"

#include "NameTypes.h"

#include <map>

class EnumStringMap : public IEnumStringMap
{
public:
	virtual void RegisterEnumString( const std::string& enumString, const int32 enumValue ) override;
	virtual int32 GetEnum( const std::string& enumName, const int32 defaultValue ) const override;

private:
	std::map<Name, int32> m_enumString;
};


void EnumStringMap::RegisterEnumString( const std::string& enumString, const int32 enumValue )
{
	m_enumString.emplace( Name( enumString ), enumValue );
}

int32 EnumStringMap::GetEnum( const std::string& enumName, const int32 defaultValue ) const
{
	auto found = m_enumString.find( Name( enumName ) );

	if ( found != std::end( m_enumString ) )
	{
		return found->second;
	}

	return defaultValue;
}

void* GetEnumStringMap()
{
	static EnumStringMap gEnumStringMap;
	return &gEnumStringMap;
}