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
	class MaterialResource final
	{
	public:
		const VertexShader* GetVertexShader( const StaticShaderSwitches* switches = nullptr ) const;
		VertexShader* GetVertexShader( const StaticShaderSwitches* switches = nullptr );
		const GeometryShader* GetGeometryShader( const StaticShaderSwitches* switches = nullptr ) const;
		GeometryShader* GetGeometryShader( const StaticShaderSwitches* switches = nullptr );
		const PixelShader* GetPixelShader( const StaticShaderSwitches* switches = nullptr ) const;
		PixelShader* GetPixelShader( const StaticShaderSwitches* switches = nullptr );
		const ShaderBase* GetShader( agl::ShaderType type ) const;

		StaticShaderSwitches GetShaderSwitches( agl::ShaderType type );

		void SetMaterial( const std::shared_ptr<Material>& material );
		const std::shared_ptr<Material> GetMaterial();
		void TakeSnapshot( DrawSnapshot& snapShot );

	private:
		void CreateGraphicsResource( const ShaderStates& shaderStates );
		void UpdateToGPU();

		std::weak_ptr<Material> m_material;

		using ConstantBufferParameter = std::pair<agl::ShaderParameter, ConstantBuffer>;
		std::vector<ConstantBufferParameter> m_materialConstantBuffers;

		using NamedShaderParameter = std::pair<agl::ShaderParameter, Name>;
		std::vector<NamedShaderParameter> m_materialConstantValueNames;
	};
}
