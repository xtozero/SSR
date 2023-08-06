#include "stdafx.h"
#include "MaterialResource.h"

#include "AbstractGraphicsInterface.h"
#include "SizedTypes.h"
#include "TaskScheduler.h"

namespace rendercore
{
	const VertexShader* MaterialResource::GetVertexShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetVertexShader( switches );
		}

		return nullptr;
	}

	VertexShader* MaterialResource::GetVertexShader( const StaticShaderSwitches* switches )
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetVertexShader( switches );
		}

		return nullptr;
	}

	const GeometryShader* MaterialResource::GetGeometryShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetGeometryShader( switches );
		}

		return nullptr;
	}

	GeometryShader* MaterialResource::GetGeometryShader( const StaticShaderSwitches* switches )
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetGeometryShader( switches );
		}

		return nullptr;
	}

	const PixelShader* MaterialResource::GetPixelShader( const StaticShaderSwitches* switches ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetPixelShader( switches );
		}

		return nullptr;
	}

	PixelShader* MaterialResource::GetPixelShader( const StaticShaderSwitches* switches )
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetPixelShader( switches );
		}

		return nullptr;
	}

	const ShaderBase* MaterialResource::GetShader( agl::ShaderType type ) const
	{
		auto material = m_material.lock();
		if ( material )
		{
			return material->GetShader( type );
		}

		return nullptr;
	}

	StaticShaderSwitches MaterialResource::GetShaderSwitches( agl::ShaderType type )
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
		CreateGraphicsResource();
	}

	void MaterialResource::TakeSnapshot( DrawSnapshot& snapShot )
	{
		auto material = m_material.lock();
		assert( material );

		// Bind constant buffer
		for ( auto& materialConstantBuffer : m_materialConstantBuffers )
		{
			auto& [cbParam, cb] = materialConstantBuffer;

			agl::SingleShaderBindings binding = snapShot.m_shaderBindings.GetSingleShaderBindings( cbParam.m_shader );

			binding.AddConstantBuffer( cbParam, cb.Resource() );
		}

		auto& graphicsInterface = GraphicsInterface();

		// Bind texture and sampler
		auto shaderTypes = { agl::ShaderType::VS, agl::ShaderType::GS, agl::ShaderType::PS };
		for ( auto shaderType : shaderTypes )
		{
			auto shader = GetShader( shaderType );
			if ( shader == nullptr )
			{
				continue;
			}

			agl::SingleShaderBindings binding = snapShot.m_shaderBindings.GetSingleShaderBindings( shaderType );

			const auto& parameterMap = shader->ParameterMap().GetParameterMap();
			for ( const auto& pair : parameterMap )
			{
				const auto& [name, param] = pair;

				if ( param.m_type == agl::ShaderParameterType::SRV ||
					param.m_type == agl::ShaderParameterType::UAV )
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
			}
		}
	}

	void MaterialResource::CreateGraphicsResource()
	{
		auto material = m_material.lock();
		if ( material == nullptr )
		{
			return;
		}

		size_t constantBufferSize = 0;
		size_t constantValueNameSize = 0;

		auto shaderTypes = {
			static_cast<uint32>( agl::ShaderType::VS ),
			static_cast<uint32>( agl::ShaderType::GS ),
			static_cast<uint32>( agl::ShaderType::PS ) };

		uint32 materialCbSlotNumbers[agl::MAX_SHADER_TYPE<uint32>];
		constexpr uint32 invalidSlot = std::numeric_limits<uint32>::max();
		std::fill( std::begin( materialCbSlotNumbers ), std::end( materialCbSlotNumbers ), invalidSlot );

		const ShaderBase* shaders[agl::MAX_SHADER_TYPE<uint32>] = {};

		// cache shader
		for ( auto shaderType : shaderTypes )
		{
			shaders[shaderType] = GetShader( static_cast<agl::ShaderType>( shaderType ) );
		}

		// find material constant buffer slot
		for ( auto shaderType : shaderTypes )
		{
			if ( auto shader = shaders[shaderType] )
			{
				const auto& parameterMap = shader->ParameterMap().GetParameterMap();
				for ( const auto& pair : parameterMap )
				{
					const auto& [name, param] = pair;
					if ( ( param.m_type == agl::ShaderParameterType::ConstantBuffer ) &&
						( name == Name( "Material" ) ) )
					{
						assert( materialCbSlotNumbers[shaderType] == invalidSlot );
						if ( materialCbSlotNumbers[shaderType] == invalidSlot )
						{
							materialCbSlotNumbers[shaderType] = param.m_bindPoint;
						}
					}
				}
			}
		}

		// gather vector size
		for ( auto shaderType : shaderTypes )
		{
			uint32 materialCbSlot = materialCbSlotNumbers[shaderType];
			if ( materialCbSlot == invalidSlot )
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

		m_materialConstantBuffers.reserve( constantBufferSize );
		m_materialConstantValueNames.reserve( constantValueNameSize );

		for ( auto shaderType : shaderTypes )
		{
			uint32 materialCbSlot = materialCbSlotNumbers[shaderType];
			if ( materialCbSlot == invalidSlot )
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
						m_materialConstantBuffers.emplace_back( param, ConstantBuffer( param.m_sizeInByte ) );
					}
					else if ( param.m_type == agl::ShaderParameterType::ConstantBufferValue )
					{
						m_materialConstantValueNames.emplace_back( param, name );
					}
				}
			}
		}

		std::sort( std::begin( m_materialConstantValueNames ), std::end( m_materialConstantValueNames ) );

		UpdateToGPU();
	}

	void MaterialResource::UpdateToGPU()
	{
		auto material = m_material.lock();
		assert( material );

		auto Update = [this, material]
		{
			for ( auto& materialConstantBuffer : m_materialConstantBuffers )
			{
				const auto& cbParam = materialConstantBuffer.first;
				auto& cb = materialConstantBuffer.second;
				char* buffer = static_cast<char*>( cb.Lock() );

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

					auto range = std::equal_range( std::begin( m_materialConstantValueNames ), std::end( m_materialConstantValueNames ), cbParam, Comp() );

					for ( auto i = range.first; i != range.second; ++i )
					{
						const auto& [param, variableName] = *i;
						char* dest = buffer + param.m_offset;

						material->CopyProperty( variableName.Str().data(), dest );
					}
				}

				cb.Unlock();
			}
		};

		EnqueueRenderTask( Update );
	}
}
