#pragma once

#include "Archive.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

using Product = std::pair<std::filesystem::path, Archive>;
using Products = std::vector<Product>;

class IManufacturer
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const = 0;
	virtual std::optional<Products> Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path& destPath ) const = 0;

	virtual ~IManufacturer() = default;
};

class AssetManufacturer
{
public:
	std::optional<Products> Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path& destPath );

	AssetManufacturer( );
	~AssetManufacturer( ) = default;
	AssetManufacturer( const AssetManufacturer& ) = delete;
	AssetManufacturer& operator=( const AssetManufacturer& ) = delete;
	AssetManufacturer( AssetManufacturer&& ) = delete;
	AssetManufacturer& operator=( AssetManufacturer&& ) = delete;

private:
	std::vector<std::unique_ptr<IManufacturer>> m_manufacturers;
};