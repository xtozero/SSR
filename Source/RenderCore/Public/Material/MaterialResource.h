#pragma once

#include "ConstantBuffer.h"
#include "Material/Material.h"
#include "NameTypes.h"
#include "ShaderParameterMap.h"

#include <memory>
#include <utility>
#include <vector>

namespace rendercore
{
	class DrawSnapshot;
	class ShadingSnapshot;

	using NamedShaderParameter = std::pair<agl::ShaderParameter, Name>;
	using NamedShaderParameterList = std::vector<NamedShaderParameter>;

	class MaterialResource final
	{
	public:
		VertexShader* GetVertexShader( const StaticShaderSwitches* switches = nullptr ) const;
		GeometryShader* GetGeometryShader( const StaticShaderSwitches* switches = nullptr ) const;
		PixelShader* GetPixelShader( const StaticShaderSwitches* switches = nullptr ) const;
		ComputeShader* GetComputeShader( const StaticShaderSwitches* switches = nullptr ) const;
		MeshShader* GetMeshShader( const StaticShaderSwitches* switches = nullptr ) const;
		AmplificationShader* GetAmplificationShader( const StaticShaderSwitches* switches = nullptr ) const;

		bool UseMeshShader() const;
		bool SupportsVisibilityRendering() const;

		StaticShaderSwitches GetShaderSwitches( agl::ShaderType type ) const;

		void SetMaterial( const std::shared_ptr<Material>& material );
		std::shared_ptr<Material> GetMaterial();

		void TakeSnapshot( DrawSnapshot& snapShot );
		void TakeSnapshot( ShadingSnapshot& snapShot );

	private:
		void CreateGraphicsResource( const ShaderBase* (&shaders)[agl::NumShaderTypes<uint32>] );
		void BindResource( const ShaderBase* shader, agl::SingleShaderBindings& binding );

		void UpdateToGPU( const ShaderBase* (&shaders)[agl::NumShaderTypes<uint32>], const NamedShaderParameterList& parametersToUpdate );

		std::weak_ptr<Material> m_material;

		using ConstantBufferParameter = std::pair<agl::ShaderParameter, ConstantBuffer>;
		std::map<size_t, ConstantBufferParameter> m_materialConstantBuffers;
	};
}
