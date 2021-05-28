#pragma once

#include "common.h"
#include "ConstantBuffer.h"
#include "DrawSnapshot.h"
#include "Material/Material.h"
#include "ShaderPrameterMap.h"

#include <memory>
#include <utility>
#include <vector>

class MaterialResource
{
public:
	void SetMaterial( const std::shared_ptr<Material>& material );
	void TakeSnapShot( DrawSnapshot& snapShot );

private:
	void CreateGraphicsResource( );
	void UpdateToGPU( );

	const VertexShader* GetVertexShader( ) const;
	const PixelShader* GetPixelShader( ) const;
	const ShaderBase* GetShader( SHADER_TYPE type ) const;

	std::shared_ptr<Material> m_material;

	using ConstantBufferParameter = std::pair<ShaderParameter, ConstantBuffer>;
	std::vector<ConstantBufferParameter> m_materialConstantBuffers;

	using NamedShaderParameter = std::pair<ShaderParameter, std::string>;
	std::vector<NamedShaderParameter> m_materialConstantValueNames;
};