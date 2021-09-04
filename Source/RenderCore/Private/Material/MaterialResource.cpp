#include "stdafx.h"
#include "MaterialResource.h"

#include "AbstractGraphicsInterface.h"
#include "MultiThread/EngineTaskScheduler.h"

void MaterialResource::SetMaterial( const std::shared_ptr<Material>& material )
{
	m_material = material;
	CreateGraphicsResource( );
}

void MaterialResource::TakeSnapShot( DrawSnapshot& snapShot )
{
	GraphicsPipelineState& pipelineState = snapShot.m_pipelineState;

	pipelineState.m_shaderState.m_vertexShader = GetVertexShader( );
	pipelineState.m_shaderState.m_pixelShader = GetPixelShader( );

	auto initializer = CreateShaderBindingsInitializer( pipelineState.m_shaderState );
	snapShot.m_shaderBindings.Initialize( initializer );

	// Bind constant buffer
	for ( auto& materialConstantBuffer : m_materialConstantBuffers )
	{
		auto& [cbParam, cb] = materialConstantBuffer;

		aga::SingleShaderBindings binding = snapShot.m_shaderBindings.GetSingleShaderBindings( cbParam.m_shader );

		binding.AddConstantBuffer( cbParam, cb.Resource( ) );
	}

	auto& graphicsInterface = GraphicsInterface( );

	// Bind texture and sampler
	auto shaderTypes = { SHADER_TYPE::VS, SHADER_TYPE::PS };
	for ( auto shaderType : shaderTypes )
	{
		auto shader = GetShader( shaderType );
		if ( shader == nullptr )
		{
			continue;
		}

		aga::SingleShaderBindings binding = snapShot.m_shaderBindings.GetSingleShaderBindings( shaderType );

		const auto& parameterMap = shader->ParameterMap( ).GetParameterMap( );
		for ( const auto& pair : parameterMap )
		{
			const auto&[name, param] = pair;

			if ( param.m_type == aga::ShaderParameterType::SRV ||
				param.m_type == aga::ShaderParameterType::UAV )
			{
				auto texture = m_material->AsTexture( name.c_str( ) );
				if ( texture == nullptr )
				{
					continue;
				}

				aga::Texture* resource = texture->Resource( );
				if ( param.m_type == aga::ShaderParameterType::SRV )
				{
					auto srv = resource ? resource->SRV( ) : nullptr;
					binding.AddSRV( param, srv );
				}
				else
				{
					auto uav = resource ? resource->UAV( ) : nullptr;
					binding.AddUAV( param, uav );
				}
			}
			else if ( param.m_type == aga::ShaderParameterType::Sampler )
			{
				if ( auto samplerOption = m_material->AsSampelrOption( name.c_str( ) ) )
				{
					auto sampler = graphicsInterface.FindOrCreate( *samplerOption );
					binding.AddSampler( param, sampler.Resource( ) );
				}
			}
		}
	}
}

void MaterialResource::CreateGraphicsResource( )
{
	if ( m_material == nullptr )
	{
		return;
	}

	std::size_t constantBufferSize = 0;
	std::size_t constantValueNameSize = 0;

	auto shaderTypes = { SHADER_TYPE::VS, SHADER_TYPE::PS };

	UINT materialCbSlotNumbers[MAX_SHADER_TYPE<int>];
	constexpr UINT invalidSlot = std::numeric_limits<UINT>::max();
	std::fill( std::begin( materialCbSlotNumbers ), std::end( materialCbSlotNumbers ), invalidSlot );

	const ShaderBase* shaders[MAX_SHADER_TYPE<int>] = {};

	// cache shader
	for ( auto shaderType : shaderTypes )
	{
		shaders[static_cast<int>( shaderType )] = GetShader( shaderType );
	}

	// find material constant buffer slot
	for ( auto shaderType : shaderTypes )
	{
		if ( auto shader = shaders[static_cast<int>( shaderType )] )
		{
			const auto& parameterMap = shader->ParameterMap( ).GetParameterMap( );
			for ( const auto& pair : parameterMap )
			{
				const auto&[name, param] = pair;
				if ( ( param.m_type == aga::ShaderParameterType::ConstantBuffer ) &&
					( name == "Material" ) )
				{
					assert( materialCbSlotNumbers[static_cast<int>( shaderType )] == invalidSlot );
					if ( materialCbSlotNumbers[static_cast<int>( shaderType )] == invalidSlot )
					{
						materialCbSlotNumbers[static_cast<int>( shaderType )] = param.m_bindPoint;
					}
				}
			}
		}
	}

	// gather vector size
	for ( auto shaderType : shaderTypes )
	{
		UINT materialCbSlot = materialCbSlotNumbers[static_cast<int>( shaderType )];
		if ( materialCbSlot == invalidSlot )
		{
			continue;
		}

		if ( auto shader = shaders[static_cast<int>( shaderType )] )
		{
			const auto& parameterMap = shader->ParameterMap( ).GetParameterMap( );
			for ( const auto& pair : parameterMap )
			{
				const auto&[name, param] = pair;
				if ( param.m_bindPoint != materialCbSlot )
				{
					continue;
				}

				if ( param.m_type == aga::ShaderParameterType::ConstantBuffer )
				{
					++constantBufferSize;
				}
				else if ( param.m_type == aga::ShaderParameterType::ConstantBufferValue )
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
		UINT materialCbSlot = materialCbSlotNumbers[static_cast<int>( shaderType )];
		if ( materialCbSlot == invalidSlot )
		{
			continue;
		}

		if ( auto shader = shaders[static_cast<int>( shaderType )] )
		{
			const auto& parameterMap = shader->ParameterMap( ).GetParameterMap( );
			for ( const auto& pair : parameterMap )
			{
				const auto&[name, param] = pair;
				if ( param.m_bindPoint != materialCbSlot )
				{
					continue;
				}

				if ( param.m_type == aga::ShaderParameterType::ConstantBuffer )
				{
					m_materialConstantBuffers.emplace_back( param, ConstantBuffer( param.m_sizeInByte ) );
				}
				else if ( param.m_type == aga::ShaderParameterType::ConstantBufferValue )
				{
					m_materialConstantValueNames.emplace_back( param, name );
				}
			}
		}
	}

	std::sort( std::begin( m_materialConstantValueNames ), std::end( m_materialConstantValueNames ) );

	UpdateToGPU( );
}

void MaterialResource::UpdateToGPU( )
{
	auto Update = [this, material = m_material]
	{
		for ( auto& materialConstantBuffer : m_materialConstantBuffers )
		{
			const auto& cbParam = materialConstantBuffer.first;
			auto& cb = materialConstantBuffer.second;
			char* buffer = static_cast<char*>( cb.Lock( ) );

			if ( buffer )
			{
				struct Comp
				{
					bool operator()( const NamedShaderParameter& lhs, const aga::ShaderParameter& rhs )
					{
						auto lVariable = std::tie( lhs.first.m_shader, lhs.first.m_bindPoint );
						auto rVariable = std::tie( rhs.m_shader, rhs.m_bindPoint );

						return lVariable < rVariable;
					}
					
					bool operator()( const aga::ShaderParameter& lhs, const NamedShaderParameter& rhs )
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

					material->CopyProperty( variableName.c_str( ), dest );
				}
			}

			cb.Unlock( );
		}
	};

	EnqueueRenderTask( Update );
}

const VertexShader* MaterialResource::GetVertexShader( ) const
{
	if ( m_material )
	{
		return m_material->GetVertexShader( );
	}

	return nullptr;
}

VertexShader* MaterialResource::GetVertexShader( )
{
	if ( m_material )
	{
		return m_material->GetVertexShader( );
	}

	return nullptr;
}

const PixelShader* MaterialResource::GetPixelShader( ) const
{
	if ( m_material )
	{
		return m_material->GetPixelShader( );
	}

	return nullptr;
}

PixelShader* MaterialResource::GetPixelShader( )
{
	if ( m_material )
	{
		return m_material->GetPixelShader( );
	}

	return nullptr;
}

const ShaderBase* MaterialResource::GetShader( SHADER_TYPE type ) const
{
	if ( m_material )
	{
		return m_material->GetShader( type );
	}

	return nullptr;
}
