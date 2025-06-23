#pragma once
#include "AssetBuilder.h"

class JsonBuilder final : public IAssetBuilder
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Build( const PathEnvironment& env, const std::filesystem::path& path ) const override;
};