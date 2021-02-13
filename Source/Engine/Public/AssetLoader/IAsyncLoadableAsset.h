#pragma once

#include "Archive.h"
#include "AssetLoader.h"
#include "common.h"

class IAsyncLoadableAsset
{
public:
	virtual ~IAsyncLoadableAsset( ) = default;
	virtual void Serialize( Archive& ar ) = 0;

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
