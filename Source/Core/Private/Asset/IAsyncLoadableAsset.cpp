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

	for ( const auto& property : properties )
	{
		property->Serialize( this, ar );
	}
}
