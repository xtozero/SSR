#pragma once

#include "AssetLoader/AssetFactory.h"
#include "Core/IAsyncLoadableAsset.h"
#include "Shader.h"

class VertexShaderAsset : public IAsyncLoadableAsset
{
public:
	virtual void LoadFromAsset( [[maybe_unused]] const JSON::Value& renderOption, [[maybe_unused]] const AssetLoaderSharedHandle& handle ) override {}
	virtual void LoadFromAsset( const char* asset, unsigned long assetSize ) override;

	RefHandle<VertexShader> m_shader;
};
DECLARE_NAMED_ASSET( VertexShaderAsset, VS );

class PixelShaderAsset : public IAsyncLoadableAsset
{
public:
	virtual void LoadFromAsset( [[maybe_unused]] const JSON::Value& renderOption, [[maybe_unused]] const AssetLoaderSharedHandle& handle ) override {}
	virtual void LoadFromAsset( const char* asset, unsigned long assetSize ) override;

	RefHandle<PixelShader> m_shader;
};
DECLARE_NAMED_ASSET( PixelShaderAsset, PS );