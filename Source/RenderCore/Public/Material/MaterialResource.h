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

namespace rendercore
{
	class MaterialResource
	{
	public:
		const VertexShader* GetVertexShader() const;
		VertexShader* GetVertexShader();
		const GeometryShader* GetGeometryShader() const;
		GeometryShader* GetGeometryShader();
		const PixelShader* GetPixelShader() const;
		PixelShader* GetPixelShader();
		const ShaderBase* GetShader( agl::ShaderType type ) const;

		void SetMaterial( const std::shared_ptr<Material>& material );
		void TakeSnapshot( DrawSnapshot& snapShot );

	private:
		void CreateGraphicsResource();
		void UpdateToGPU();

		std::shared_ptr<Material> m_material;

		using ConstantBufferParameter = std::pair<agl::ShaderParameter, ConstantBuffer>;
		std::vector<ConstantBufferParameter> m_materialConstantBuffers;

		using NamedShaderParameter = std::pair<agl::ShaderParameter, Name>;
		std::vector<NamedShaderParameter> m_materialConstantValueNames;
	};
}
