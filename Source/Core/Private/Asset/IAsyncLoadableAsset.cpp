#include "IAsyncLoadableAsset.h"

Archive& operator<<(Archive& ar, AssetHeader& header)
{
	ar << header.m_assetId;

	ar << header.m_headerVersion;
	ar << header.m_assetVersion;

	ar << header.m_lastWriteTime;
	ar << header.m_fileHash;

	return ar;
}

void AsyncLoadableAsset::SetPath( const std::filesystem::path& path )
{
	m_path = path;
}

void AsyncLoadableAsset::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode() )
	{
		m_header.m_assetId = GetId();
		m_header.m_assetVersion = GetAssetVersion();
	}

	ar << m_header;

	Super::Serialize( ar );
}

void AsyncLoadableAsset::SetLastWriteTime( std::filesystem::file_time_type lastWriteTime )
{
	m_header.m_lastWriteTime = lastWriteTime;
}

void AsyncLoadableAsset::SetFileHash( uint64 fileHash )
{
	m_header.m_fileHash = fileHash;
}
