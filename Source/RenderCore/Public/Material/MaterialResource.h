#pragma once

#include "common.h"
#include "ConstantBuffer.h"
#include "DrawSnapshot.h"
#include "Material/Material.h"
#include "NameTypes.h"
#include "ShaderParameterMap.h"

#include <memory>
#include <utility>
#include <vector>

class MaterialResource
{
public:
	const VertexShader* GetVertexShader() const;
	VertexShader* GetVertexShader();
	const GeometryShader* GetGeometryShader() const;
	GeometryShader* GetGeometryShader();
	const PixelShader* GetPixelShader() const;
	PixelShader* GetPixelShader();
	const ShaderBase* GetShader( SHADER_TYPE type ) const;

	void SetMaterial( const std::shared_ptr<Material>& material );
	void TakeSnapshot( DrawSnapshot& snapShot );

private:
	void CreateGraphicsResource();
	void UpdateToGPU();

	std::shared_ptr<Material> m_material;

	using ConstantBufferParameter = std::pair<aga::ShaderParameter, ConstantBuffer>;
	std::vector<ConstantBufferParameter> m_materialConstantBuffers;

	using NamedShaderParameter = std::pair<aga::ShaderParameter, Name>;
	std::vector<NamedShaderParameter> m_materialConstantValueNames;
};