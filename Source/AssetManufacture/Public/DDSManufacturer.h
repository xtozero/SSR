#pragma once
#include "AssetManufacturer.h"

class DDSManufacturer : public IManufacturer
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Manufacture( const PathEnvironment& env, const std::filesystem::path& path ) const override;
};