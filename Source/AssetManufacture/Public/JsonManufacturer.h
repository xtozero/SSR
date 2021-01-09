#pragma once
#include "AssetManufacturer.h"

class JsonManufacturer : public IManufacturer
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path* destRootHint = nullptr ) const override;
};