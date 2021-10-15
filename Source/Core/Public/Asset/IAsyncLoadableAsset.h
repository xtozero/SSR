#pragma once

#include "Archive.h"
#include "AssetLoader.h"
#include "common.h"

#include <memory>

class IAsyncLoadableAsset : public std::enable_shared_from_this<IAsyncLoadableAsset>
{
public:
	virtual ~IAsyncLoadableAsset( ) = default;
	virtual void Serialize( Archive& ar ) = 0;

	std::shared_ptr<IAsyncLoadableAsset> SharedThis( )
	{
		return shared_from_this( );
	}

protected:
	virtual void PostLoadImpl( ) = 0;
};

class AsyncLoadableAsset : public IAsyncLoadableAsset
{
public:
	void PostLoad( )
	{
		PostLoadImpl( );
	}
};
