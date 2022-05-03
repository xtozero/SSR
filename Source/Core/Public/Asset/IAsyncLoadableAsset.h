#pragma once

#include "Archive.h"
#include "AssetLoader.h"
#include "common.h"
#include "Reflection.h"

#include <filesystem>
#include <memory>

class IAsyncLoadableAsset : public std::enable_shared_from_this<IAsyncLoadableAsset>
{
	GENERATE_CLASS_TYPE_INFO( IAsyncLoadableAsset );

public:
	virtual ~IAsyncLoadableAsset() = default;
	virtual void SetPath( const std::filesystem::path& path ) = 0;
	virtual void Serialize( Archive& ar ) = 0;

	std::shared_ptr<IAsyncLoadableAsset> SharedThis()
	{
		return shared_from_this();
	}

protected:
	virtual void PostLoadImpl() = 0;
};

class AsyncLoadableAsset : public IAsyncLoadableAsset
{
	GENERATE_CLASS_TYPE_INFO( AsyncLoadableAsset );

public:
	const std::filesystem::path& Path() const 
	{ 
		return m_path; 
	}

	virtual void SetPath( const std::filesystem::path& path ) override
	{ 
		m_path = path; 
	}

	void PostLoad()
	{
		PostLoadImpl();
	}

private:
	std::filesystem::path m_path;
};
