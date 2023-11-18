#include "IAsyncLoadableAsset.h"

void AsyncLoadableAsset::SetPath( const std::filesystem::path& path )
{
	m_path = path;
}

void AsyncLoadableAsset::Serialize( Archive& ar )
{
	const TypeInfo& typeInfo = GetTypeInfo();
	const auto& properties = typeInfo.GetProperties();
	
	if ( ar.IsWriteMode() )
	{
		ar << GetID();
	}
	else
	{
		uint32 id;
		ar << id;
	}

	ar << m_lastWriteTime;

	for ( const auto& property : properties )
	{
		property->Serialize( this, ar );
	}
}

void AsyncLoadableAsset::SetLastWriteTime( std::filesystem::file_time_type lastWriteTime )
{
	m_lastWriteTime = lastWriteTime;
}
