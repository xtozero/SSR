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
		VertexShader* GetVertexShader( const IShaderPermutation* permutation = nullptr ) const;
		GeometryShader* GetGeometryShader( const IShaderPermutation* permutation = nullptr ) const;
		PixelShader* GetPixelShader( const IShaderPermutation* permutation = nullptr ) const;
		ComputeShader* GetComputeShader( const IShaderPermutation* permutation = nullptr ) const;
		MeshShader* GetMeshShader( const IShaderPermutation* permutation = nullptr ) const;
		AmplificationShader* GetAmplificationShader( const IShaderPermutation* permutation = nullptr ) const;

		bool UseMeshShader() const;
		bool SupportsVisibilityRendering() const;

		IShaderPermutation& GetShaderPermutation( agl::ShaderType type ) const;

		void SetMaterial( const std::shared_ptr<Material>& material );
		std::shared_ptr<Material> GetMaterial();

		void TakeSnapshot( DrawSnapshot& snapShot );
		void TakeSnapshot( ShadingSnapshot& snapShot );

	private:
		void CreateGraphicsResource( const ShaderBase* (&shaders)[agl::NumNonRTShaderTypes<uint32>] );
		void BindResource( const ShaderBase* shader, agl::SingleShaderBindings& binding );

		void UpdateToGPU( const ShaderBase* (&shaders)[agl::NumNonRTShaderTypes<uint32>], const NamedShaderParameterList& parametersToUpdate );

		std::weak_ptr<Material> m_material;

		using ConstantBufferParameter = std::pair<agl::ShaderParameter, ConstantBuffer>;
		std::map<size_t, ConstantBufferParameter> m_materialConstantBuffers;
	};
}
