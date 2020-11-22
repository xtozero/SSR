#pragma once

#include "AssetLoader/AssetLoader.h"

namespace JSON
{
	class Value;
}

class IAsyncLoadableAsset
{
public:
	virtual ~IAsyncLoadableAsset( ) = default;
	virtual void LoadFromAsset( const JSON::Value& renderOption, const AssetLoaderSharedHandle& handle ) = 0;
	virtual void LoadFromAsset( const char* asset, unsigned long assetSize ) = 0;
};