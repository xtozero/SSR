#pragma once
#include "AssetManufacturer.h"

class WavefrontObjManufacturer : public IManufacturer
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path& destPath ) const override;
};

class WavefrontMtlManufacturer : public IManufacturer
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path& destPath ) const override;
};