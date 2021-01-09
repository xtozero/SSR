#pragma once

#include "Archive.h"
#include "AssetLoader/AssetLoader.h"
#include "common.h"

namespace JSON
{
	class Value;
}

class IAsyncLoadableAsset
{
public:
	virtual ~IAsyncLoadableAsset( ) = default;
	virtual void Serialize( Archive& ar ) = 0;
};

class AsyncLoadableAsset : public IAsyncLoadableAsset
{
};
