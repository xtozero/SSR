#include "MaterialResource.h"

#include "AbstractGraphicsInterface.h"
#include "DrawSnapshot.h"
#include "ShadingSnapshot.h"
#include "SizedTypes.h"
#include "TaskScheduler.h"

using ::rendercore::GraphicsPipelineState;
using ::rendercore::ShaderBase;

namespace
{
	const ShaderBase* GetShader( const GraphicsPipelineState& pipelineState, agl::ShaderType type )
	{
		switch ( type )
		{
		case agl::ShaderType::None:
			break;
		case agl::ShaderType::Vertex:
			return pipelineState.m_shaderState.m_vertexShader;
			break;
		case agl::ShaderType::Hull:
			break;
		case agl::ShaderType::Domain:
			break;
		case agl::ShaderType::Geometry:
			return pipelineState.m_shaderState.m_geometryShader;
			break;
		case agl::ShaderType::Pixel:
			return pipelineState.m_shaderState.m_pixelShader;
			break;
		case agl::ShaderType::Compute:
			break;
		case agl::ShaderType::Mesh:
			return pipelineState.m_shaderState.m_meshShader;
			break;
		case agl::ShaderType::Amplification:
			return pipelineState.m_shaderState.m_amplificationShader;
			break;
		case agl::ShaderType::Count:
			[[fallthrough]];
		default:
			break;
		}

		return nullptr;
	}
}

namespace rendercore
{
	VertexShader* MaterialResource::GetVertexShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetVertexShader( switches );
		}

		return nullptr;
	}

	GeometryShader* MaterialResource::GetGeometryShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetGeometryShader( switches );
		}

		return nullptr;
	}

	PixelShader* MaterialResource::GetPixelShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetPixelShader( switches );
		}

		return nullptr;
	}

	ComputeShader* MaterialResource::GetComputeShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetComputeShader( switches );
		}

		return nullptr;
	}

	MeshShader* MaterialResource::GetMeshShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetMeshShader( switches );
		}

		return nullptr;
	}

	AmplificationShader* MaterialResource::GetAmplificationShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetAmplificationShader( switches );
		}

		return nullptr;
	}

	bool MaterialResource::UseMeshShader() const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->UseMeshShader();
		}

		return false;
	}

	bool MaterialResource::SupportsVisibilityRendering() const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->SupportsVisibilityRendering();
		}

		return false;
	}

	StaticShaderSwitches MaterialResource::GetShaderSwitches( agl::ShaderType type ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetShaderSwitches( type );
		}

		return StaticShaderSwitches();
	}

	void MaterialResource::SetMaterial( const std::shared_ptr<Material>& material )
	{
		m_material = material;
	}

	std::shared_ptr<Material> MaterialResource::GetMaterial()
	{
		return m_material.lock();
	}

	void MaterialResource::TakeSnapshot( DrawSnapshot& snapShot )
	{
		const ShaderStates& shaderState = snapShot.m_pipelineState.m_shaderState;
		const ShaderBase* shaders[agl::NumGraphicsShaderTypes<uint32>] = {
			shaderState.m_vertexShader,
			nullptr,
			nullptr,
			shaderState.m_geometryShader,
			shaderState.m_pixelShader,
			nullptr,
			shaderState.m_meshShader,
			shaderState.m_amplificationShader
		};

		CreateGraphicsResource( shaders );

		// Bind resources
		constexpr agl::ShaderType ShaderTypes[] = {
			agl::ShaderType::Vertex,
			agl::ShaderType::Geometry,
			agl::ShaderType::Pixel,
			agl::ShaderType::Mesh,
			agl::ShaderType::Amplification };

		for ( auto shaderType : ShaderTypes )
		{
			auto shader = ::GetShader( snapShot.m_pipelineState, shaderType );
			if ( shader == nullptr )
			{
				continue;
			}

			agl::SingleShaderBindings binding = snapShot.m_shaderBindings.GetSingleShaderBindings( shaderType );
			BindResource( shader, binding );
		}
	}

	void MaterialResource::TakeSnapshot( ShadingSnapshot& snapShot )
	{
		if ( snapShot.m_computeShader == nullptr )
		{
			return;
		}

		auto material = m_material.lock();
		assert( material );

		const ShaderBase* shaders[agl::NumGraphicsShaderTypes<uint32>] = {
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			snapShot.m_computeShader,
			nullptr,
			nullptr
		};

		CreateGraphicsResource( shaders );

		agl::SingleShaderBindings binding = snapShot.m_shaderBindings.GetSingleShaderBindings( agl::ShaderType::Compute );
		BindResource( snapShot.m_computeShader, binding );
	}

	void MaterialResource::CreateGraphicsResource( const ShaderBase* (&shaders)[agl::NumGraphicsShaderTypes<uint32>] )
	{
		auto material = m_material.lock();
		if ( material == nullptr )
		{
			return;
		}

		size_t constantBufferSize = 0;
		size_t constantValueNameSize = 0;

		constexpr uint32 ShaderTypes[] = {
			static_cast<uint32>( agl::ShaderType::Vertex ),
			static_cast<uint32>( agl::ShaderType::Geometry ),
			static_cast<uint32>( agl::ShaderType::Pixel ),
			static_cast<uint32>( agl::ShaderType::Compute ),
			static_cast<uint32>( agl::ShaderType::Mesh ),
			static_cast<uint32>( agl::ShaderType::Amplification ) };

		uint32 materialCbSlotNumbers[agl::NumGraphicsShaderTypes<uint32>];
		constexpr uint32 InvalidSlot = std::numeric_limits<uint32>::max();
		std::ranges::fill( materialCbSlotNumbers, InvalidSlot );

		// find material constant buffer slot
		for ( auto shaderType : ShaderTypes )
		{
			if ( auto shader = shaders[shaderType] )
			{
				const auto& parameterMap = shader->ParameterMap().GetParameterMap();
				for ( const auto& pair : parameterMap )
				{
					const auto& [name, param] = pair;
					if ( ( param.m_type == agl::ShaderParameterType::ConstantBuffer ) &&
						( name == StaticName( "Material" ) ) )
					{
						assert( materialCbSlotNumbers[shaderType] == InvalidSlot );
						if ( materialCbSlotNumbers[shaderType] == InvalidSlot )
						{
							materialCbSlotNumbers[shaderType] = param.m_bindPoint;
						}
					}
				}
			}
		}

		// gather vector size
		for ( auto shaderType : ShaderTypes )
		{
			uint32 materialCbSlot = materialCbSlotNumbers[shaderType];
			if ( materialCbSlot == InvalidSlot )
			{
				continue;
			}

			if ( auto shader = shaders[shaderType] )
			{
				const auto& parameterMap = shader->ParameterMap().GetParameterMap();
				for ( const auto& pair : parameterMap )
				{
					const auto& [name, param] = pair;
					if ( param.m_bindPoint != materialCbSlot )
					{
						continue;
					}

					if ( param.m_type == agl::ShaderParameterType::ConstantBuffer )
					{
						++constantBufferSize;
					}
					else if ( param.m_type == agl::ShaderParameterType::ConstantBufferValue )
					{
						++constantValueNameSize;
					}
				}
			}
		}

		if ( ( constantBufferSize == 0 ) && ( constantValueNameSize == 0 ) )
		{
			return;
		}

		NamedShaderParameterList parametersToUpdate;
		parametersToUpdate.reserve( constantValueNameSize );

		for ( auto shaderType : ShaderTypes )
		{
			uint32 materialCbSlot = materialCbSlotNumbers[shaderType];
			if ( materialCbSlot == InvalidSlot )
			{
				continue;
			}

			if ( auto shader = shaders[shaderType] )
			{
				const auto& parameterMap = shader->ParameterMap().GetParameterMap();
				for ( const auto& pair : parameterMap )
				{
					const auto& [name, param] = pair;
					if ( param.m_bindPoint != materialCbSlot )
					{
						continue;
					}

					if ( param.m_type == agl::ShaderParameterType::ConstantBuffer )
					{
						m_materialConstantBuffers.emplace( shader->GetHash(), std::make_pair( param, ConstantBuffer( param.m_sizeInByte ) ) );
					}
					else if ( param.m_type == agl::ShaderParameterType::ConstantBufferValue )
					{
						parametersToUpdate.emplace_back( param, name );
					}
				}
			}
		}

		std::ranges::sort( parametersToUpdate );

		UpdateToGPU( shaders, parametersToUpdate );
	}

	void MaterialResource::BindResource( const ShaderBase* shader, agl::SingleShaderBindings& binding )
	{
		auto material = m_material.lock();
		assert( material );

		auto& graphicsInterface = GraphicsInterface();

		const auto& foundCB = m_materialConstantBuffers.find( shader->GetHash() );
		if ( foundCB != std::end( m_materialConstantBuffers ) )
		{
			auto& [cbParam, cb] = foundCB->second;
			binding.AddConstantBuffer( cbParam, cb.Resource() );
		}

		const auto& parameterMap = shader->ParameterMap().GetParameterMap();
		for ( const auto& pair : parameterMap )
		{
			const auto& [name, param] = pair;

			if ( param.m_type == agl::ShaderParameterType::SRV
				|| param.m_type == agl::ShaderParameterType::UAV )
			{
				auto texture = material->AsTexture( name.Str().data() );
				if ( texture == nullptr )
				{
					continue;
				}

				agl::Texture* resource = texture->Resource();
				if ( param.m_type == agl::ShaderParameterType::SRV )
				{
					auto srv = resource ? resource->SRV() : nullptr;
					binding.AddSRV( param, srv );
				}
				else
				{
					auto uav = resource ? resource->UAV() : nullptr;
					binding.AddUAV( param, uav );
				}
			}
			else if ( param.m_type == agl::ShaderParameterType::Sampler )
			{
				if ( auto samplerOption = material->AsSampelrOption( name.Str().data() ) )
				{
					auto sampler = graphicsInterface.FindOrCreate( *samplerOption );
					binding.AddSampler( param, sampler.Resource() );
				}
			}
			else if ( param.m_type == agl::ShaderParameterType::Bindless )
			{
				if ( auto samplerOption = material->AsSampelrOption( name.Str().data() ) )
				{
					auto sampler = graphicsInterface.FindOrCreate( *samplerOption );
					binding.AddBindless( param, sampler.Resource() );
				}
				else if ( auto texture = material->AsTexture( name.Str().data() ) )
				{
					agl::Texture* resource = texture->Resource();
					auto srv = resource ? resource->SRV() : nullptr;
					binding.AddBindless( param, srv );
				}
			}
		}
	}

	void MaterialResource::UpdateToGPU( const ShaderBase* (&shaders)[agl::NumGraphicsShaderTypes<uint32>], const NamedShaderParameterList& parametersToUpdate )
	{
		assert( IsInRenderThread() );

		auto material = m_material.lock();
		assert( material );

		for ( auto shader : shaders )
		{
			if ( shader == nullptr )
			{
				continue;
			}

			auto foundCB = m_materialConstantBuffers.find( shader->GetHash() );
			if ( foundCB != std::end( m_materialConstantBuffers ) )
			{
				const auto& cbParam = foundCB->second.first;
				auto& cb = foundCB->second.second;
				auto buffer = static_cast<char*>( cb.Lock() );

				if ( buffer )
				{
					std::memset( buffer, 0, cb.Size() );

					struct Comp
					{
						bool operator()( const NamedShaderParameter& lhs, const agl::ShaderParameter& rhs )
						{
							auto lVariable = std::tie( lhs.first.m_shader, lhs.first.m_bindPoint );
							auto rVariable = std::tie( rhs.m_shader, rhs.m_bindPoint );

							return lVariable < rVariable;
						}

						bool operator()( const agl::ShaderParameter& lhs, const NamedShaderParameter& rhs )
						{
							auto lVariable = std::tie( lhs.m_shader, lhs.m_bindPoint );
							auto rVariable = std::tie( rhs.first.m_shader, rhs.first.m_bindPoint );

							return lVariable < rVariable;
						}
					};

					auto range = std::equal_range( std::begin( parametersToUpdate ), std::end( parametersToUpdate ), cbParam, Comp() );

					for ( auto i = range.first; i != range.second; ++i )
					{
						const auto& [param, variableName] = *i;
						char* dest = buffer + param.m_offset;

						material->CopyProperty( variableName.Str().data(), dest );
					}
				}

				cb.Unlock();
			}
		}
	}
}
