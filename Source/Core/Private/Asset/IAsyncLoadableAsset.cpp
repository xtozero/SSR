#include "IAsyncLoadableAsset.h"

void AsyncLoadableAsset::SetPath( const std::filesystem::path& path )
{
	m_path = path;
}

void AsyncLoadableAsset::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode() )
	{
		ar << GetId();
	}
	else
	{
		uint32 id;
		ar << id;
	}

	ar << m_lastWriteTime;

	Super::Serialize( ar );
}

void AsyncLoadableAsset::SetLastWriteTime( std::filesystem::file_time_type lastWriteTime )
{
	m_lastWriteTime = lastWriteTime;
}
