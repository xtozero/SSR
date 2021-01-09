#pragma once

#include "AssetLoader/AssetFactory.h"
#include "Core/IAsyncLoadableAsset.h"
#include "Shader.h"

class VertexShaderAsset : public AsyncLoadableAsset
{
	DECLARE_ASSET( VertexShaderAsset );
public:
	virtual void Serialize( Archive& ar ) override;

	RefHandle<VertexShader> m_shader;
};

class PixelShaderAsset : public AsyncLoadableAsset
{
	DECLARE_ASSET( PixelShaderAsset );
public:
	virtual void Serialize( Archive& ar ) override;

	RefHandle<PixelShader> m_shader;
};
