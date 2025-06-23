#pragma once

#include "Serializable.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

using Product = std::pair<std::filesystem::path, std::unique_ptr<Serializable>>;
using Products = std::vector<Product>;

struct PathEnvironment;

class IAssetBuilder
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const = 0;
	virtual std::optional<Products> Build( const PathEnvironment& env, const std::filesystem::path& path ) const = 0;

	virtual bool Initialize() { return true; }

	virtual ~IAssetBuilder() = default;
};

class AssetBuilder final
{
public:
	std::optional<Products> Build( const PathEnvironment& env, const std::filesystem::path& path, uint64 fileHash ) const;

	void Initialize();

	AssetBuilder() = default;
	~AssetBuilder() = default;
	AssetBuilder( const AssetBuilder& ) = delete;
	AssetBuilder& operator=( const AssetBuilder& ) = delete;
	AssetBuilder( AssetBuilder&& ) = delete;
	AssetBuilder& operator=( AssetBuilder&& ) = delete;

private:
	std::vector<std::unique_ptr<IAssetBuilder>> m_assetBuilders;
};