#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "AssetLoader.h"
#include "Reflection.h"
#include "Serializable.h"

#include <filesystem>
#include <memory>

class IAsyncLoadableAsset : public std::enable_shared_from_this<IAsyncLoadableAsset>, public Serializable
{
	GENERATE_CLASS_TYPE_INFO( IAsyncLoadableAsset );

public:
	virtual uint32 GetId() const = 0;
	virtual uint32 GetAssetVersion() const = 0;

	virtual void SetPath( const std::filesystem::path& path ) = 0;

	std::shared_ptr<IAsyncLoadableAsset> SharedThis()
	{
		return shared_from_this();
	}

	virtual ~IAsyncLoadableAsset() = default;

protected:
	virtual void PostLoadImpl() = 0;
};

struct AssetHeader
{
	uint32 m_assetId = 0;

	uint32 m_headerVersion = 0;
	uint32 m_assetVersion = 0;

	std::filesystem::file_time_type m_lastWriteTime;
	uint64 m_fileHash = ( std::numeric_limits<uint64>::max )();

	friend Archive& operator<<(Archive& ar, AssetHeader& header);
};

class AsyncLoadableAsset : public IAsyncLoadableAsset
{
	GENERATE_CLASS_TYPE_INFO( AsyncLoadableAsset );

public:
	virtual void SetPath( const std::filesystem::path& path ) override;
	virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path() const 
	{ 
		return m_path; 
	}

	std::filesystem::file_time_type LastWriteTime() const
	{
		return m_header.m_lastWriteTime;
	}

	uint64 GetFileHash() const
	{
		return m_header.m_fileHash;
	}

	void SetLastWriteTime( std::filesystem::file_time_type lastWriteTime );
	void SetFileHash( uint64 fileHash );

	void PostLoad()
	{
		PostLoadImpl();
	}

private:
	std::filesystem::path m_path;
	AssetHeader m_header;
};
