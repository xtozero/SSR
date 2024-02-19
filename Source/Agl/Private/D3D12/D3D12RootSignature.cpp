#include "D3D12RootSignature.h"

#include "D3D12Api.h"
#include "D3D12Shaders.h"

#include "PipelineState.h"

#include <wrl/client.h>

using namespace ::Microsoft::WRL;

namespace agl
{
	D3D12_SHADER_VISIBILITY GetShaderVisibility( ShaderType shaderType )
	{
		switch ( shaderType )
		{
		case agl::ShaderType::VS:
			return D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		case agl::ShaderType::HS:
			return D3D12_SHADER_VISIBILITY_HULL;
			break;
		case agl::ShaderType::DS:
			return D3D12_SHADER_VISIBILITY_DOMAIN;
			break;
		case agl::ShaderType::GS:
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
			break;
		case agl::ShaderType::PS:
			return D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		case agl::ShaderType::CS:
			return D3D12_SHADER_VISIBILITY_ALL;
			break;
		default:
			break;
		}

		assert( false && "GetShaderVisibility - Unsurpported ShaderType" );
		return D3D12_SHADER_VISIBILITY_ALL;
	}

	ResourceStatistics SurveyShader( const ShaderBase& shader )
	{
		const ShaderParameterInfo& paramInfo = shader.GetParameterInfo();
		return { static_cast<uint32>( paramInfo.m_srvs.size() )
			, static_cast<uint32>( paramInfo.m_uavs.size() )
			, static_cast<uint32>( paramInfo.m_constantBuffers.size() )
			, static_cast<uint32>( paramInfo.m_samplers.size() ) };
	}

	ResourceStatistics SurveyPipeline( const GraphicsPipelineStateInitializer& initializer )
	{
		ResourceStatistics statistics;

		auto vertexShader = static_cast<D3D12VertexShader*>( initializer.m_vertexShader );
		if ( vertexShader )
		{
			statistics += SurveyShader( *vertexShader );
		}

		auto geometryShader = static_cast<D3D12GeometryShader*>( initializer.m_geometryShader );
		if ( geometryShader )
		{
			statistics += SurveyShader( *geometryShader );
		}

		auto pixelShader = static_cast<D3D12PixelShader*>( initializer.m_piexlShader );
		if ( pixelShader )
		{
			statistics += SurveyShader( *pixelShader );
		}

		return statistics;
	}

	ResourceStatistics SurveyPipeline( const ComputePipelineStateInitializer& initializer )
	{
		auto computeShader = static_cast<D3D12ComputeShader*>( initializer.m_computeShader );
		if ( computeShader )
		{
			return SurveyShader( *computeShader );
		}

		return {};
	}

	ID3D12RootSignature* D3D12RootSignature::Resource() const
	{
		return m_rootSignature;
	}

	D3D12RootSignature::D3D12RootSignature( const GraphicsPipelineStateInitializer& initializer )
	{
		InlineShaderArray shaders;
		if ( initializer.m_vertexShader )
		{
			shaders.emplace_back( ShaderType::VS, static_cast<D3D12VertexShader*>( initializer.m_vertexShader ) );
		}

		if ( initializer.m_geometryShader )
		{
			shaders.emplace_back( ShaderType::GS, static_cast<D3D12GeometryShader*>( initializer.m_geometryShader ) );
		}

		if ( initializer.m_piexlShader )
		{
			shaders.emplace_back( ShaderType::PS, static_cast<D3D12PixelShader*>( initializer.m_piexlShader ) );
		}

		bool hasBindless = false;
		for ( const auto& [type, shader] : shaders )
		{
			const ShaderParameterInfo& paramInfo = shader->GetParameterInfo();
			if ( paramInfo.m_bindless.size() > 0 )
			{
				hasBindless = true;
				break;
			}
		}

		ResourceStatistics statistics = SurveyPipeline( initializer );
		m_parameters.reserve( hasBindless ? ( statistics.Total() + 2 ) : statistics.NumResourceCategory() );
		m_descritorRange.reserve( statistics.Total() + ( hasBindless ? 5 : 0 ) );

		if ( hasBindless )
		{
			InitializeForBindless( shaders );
		}
		else
		{
			for ( const auto& [type, shader] : shaders )
			{
				const ShaderParameterInfo& paramInfo = shader->GetParameterInfo();
				InitializeSRV( type, paramInfo );
			}

			for ( const auto& [type, shader] : shaders )
			{
				const ShaderParameterInfo& paramInfo = shader->GetParameterInfo();
				InitializeUAV( type, paramInfo );
			}

			for ( const auto& [type, shader] : shaders )
			{
				const ShaderParameterInfo& paramInfo = shader->GetParameterInfo();
				InitializeCB( type, paramInfo );
			}

			for ( const auto& [type, shader] : shaders )
			{
				const ShaderParameterInfo& paramInfo = shader->GetParameterInfo();
				InitializeSampler( type, paramInfo );
			}
		}

		m_desc.NumParameters = static_cast<uint32>( m_parameters.size() );
		m_desc.pParameters = m_parameters.data();
		m_desc.NumStaticSamplers = 0;
		m_desc.pStaticSamplers = nullptr;
		m_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}

	D3D12RootSignature::D3D12RootSignature( const ComputePipelineStateInitializer& initializer )
	{
		auto computeShader = static_cast<D3D12ComputeShader*>( initializer.m_computeShader );
		if ( computeShader == nullptr )
		{
			return;
		}

		const ShaderParameterInfo& paramInfo = computeShader->GetParameterInfo();
		bool hasBindless = paramInfo.m_bindless.size() > 0;

		ResourceStatistics statistics = SurveyPipeline( initializer );
		m_parameters.reserve( hasBindless ? ( statistics.Total() + 2 ) : statistics.NumResourceCategory() );
		m_descritorRange.reserve( statistics.Total() + ( hasBindless ? 5 : 0 ) );

		if ( hasBindless )
		{
			InlineShaderArray shaders;
			shaders.emplace_back( ShaderType::CS, computeShader );

			InitializeForBindless( shaders );
		}
		else
		{
			InitializeSRV( ShaderType::CS, paramInfo );
			InitializeUAV( ShaderType::CS, paramInfo );
			InitializeCB( ShaderType::CS, paramInfo );
			InitializeSampler( ShaderType::CS, paramInfo );
		}

		m_desc.NumParameters = static_cast<uint32>( m_parameters.size() );
		m_desc.pParameters = m_parameters.data();
		m_desc.NumStaticSamplers = 0;
		m_desc.pStaticSamplers = nullptr;
		m_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	}

	void D3D12RootSignature::InitResource()
	{
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		HRESULT hr = D3D12SerializeRootSignature( &m_desc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf() );
		assert( SUCCEEDED( hr ) && "Fail to serialize root signature" );

		hr = D3D12Device().CreateRootSignature( 0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS( &m_rootSignature ) );
		assert( SUCCEEDED( hr ) && "Fail to create root signature" );
	}

	void D3D12RootSignature::FreeResource()
	{
		if ( m_rootSignature )
		{
			m_rootSignature->Release();
			m_rootSignature = nullptr;
		}
	}

	void D3D12RootSignature::InitializeSRV( ShaderType shaderType, const ShaderParameterInfo& paramInfo )
	{
		if ( paramInfo.m_srvs.empty() )
		{
			return;
		}

		size_t rangeBase = m_descritorRange.size();

		for ( const ShaderParameter& srvParam : paramInfo.m_srvs )
		{
			m_descritorRange.emplace_back();
			D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.back();

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.NumDescriptors = 1;
			range.BaseShaderRegister = srvParam.m_bindPoint;
			range.RegisterSpace = srvParam.m_space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		m_parameters.emplace_back();
		D3D12_ROOT_PARAMETER& param = m_parameters.back();

		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = GetShaderVisibility( shaderType );
		param.DescriptorTable.NumDescriptorRanges = static_cast<uint32>( paramInfo.m_srvs.size() );
		param.DescriptorTable.pDescriptorRanges = &m_descritorRange[rangeBase];
	}

	void D3D12RootSignature::InitializeUAV( ShaderType shaderType, const ShaderParameterInfo& paramInfo )
	{
		if ( paramInfo.m_uavs.empty() )
		{
			return;
		}

		size_t rangeBase = m_descritorRange.size();

		for ( const ShaderParameter& uavParam : paramInfo.m_uavs )
		{
			m_descritorRange.emplace_back();
			D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.back();

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			range.NumDescriptors = 1;
			range.BaseShaderRegister = uavParam.m_bindPoint;
			range.RegisterSpace = uavParam.m_space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		m_parameters.emplace_back();
		D3D12_ROOT_PARAMETER& param = m_parameters.back();

		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = GetShaderVisibility( shaderType );
		param.DescriptorTable.NumDescriptorRanges = static_cast<uint32>( paramInfo.m_uavs.size() );
		param.DescriptorTable.pDescriptorRanges = &m_descritorRange[rangeBase];
	}

	void D3D12RootSignature::InitializeCB( ShaderType shaderType, const ShaderParameterInfo& paramInfo )
	{
		if ( paramInfo.m_constantBuffers.empty() )
		{
			return;
		}

		size_t rangeBase = m_descritorRange.size();

		for ( const ShaderParameter& constantBufferParam : paramInfo.m_constantBuffers )
		{
			m_descritorRange.emplace_back();
			D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.back();

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			range.NumDescriptors = 1;
			range.BaseShaderRegister = constantBufferParam.m_bindPoint;
			range.RegisterSpace = constantBufferParam.m_space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		m_parameters.emplace_back();
		D3D12_ROOT_PARAMETER& param = m_parameters.back();

		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = GetShaderVisibility( shaderType );
		param.DescriptorTable.NumDescriptorRanges = static_cast<uint32>( paramInfo.m_constantBuffers.size() );
		param.DescriptorTable.pDescriptorRanges = &m_descritorRange[rangeBase];
	}

	void D3D12RootSignature::InitializeSampler( ShaderType shaderType, const ShaderParameterInfo& paramInfo )
	{
		if ( paramInfo.m_samplers.empty() )
		{
			return;
		}

		size_t rangeBase = m_descritorRange.size();

		for ( const ShaderParameter& samplerParam : paramInfo.m_samplers )
		{
			m_descritorRange.emplace_back();
			D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.back();

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			range.NumDescriptors = 1;
			range.BaseShaderRegister = samplerParam.m_bindPoint;
			range.RegisterSpace = samplerParam.m_space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}

		m_parameters.emplace_back();
		D3D12_ROOT_PARAMETER& param = m_parameters.back();

		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = GetShaderVisibility( shaderType );
		param.DescriptorTable.NumDescriptorRanges = static_cast<uint32>( paramInfo.m_samplers.size() );
		param.DescriptorTable.pDescriptorRanges = &m_descritorRange[rangeBase];
	}

	void D3D12RootSignature::InitializeForBindless( InlineShaderArray& shaders )
	{
		{
			constexpr int32 MaxStandardSrvCount = 4;
			for ( int32 i = 0; i < MaxStandardSrvCount; ++i )
			{
				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				range.NumDescriptors = (uint32)-1;
				range.BaseShaderRegister = 0;
				range.RegisterSpace = 100 + i;
				range.OffsetInDescriptorsFromTableStart = 0;
			}

			D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.NumDescriptorRanges = MaxStandardSrvCount;
			param.DescriptorTable.pDescriptorRanges = &m_descritorRange[0];
		}

		{
			D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			range.NumDescriptors = (uint32)-1;
			range.BaseShaderRegister = 0;
			range.RegisterSpace = 100;
			range.OffsetInDescriptorsFromTableStart = 0;

			D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.DescriptorTable.pDescriptorRanges = &range;
		}

		for ( auto& [type, shader] : shaders )
		{
			const ShaderParameterInfo& paramInfo = shader->GetParameterInfo();
			for ( const ShaderParameter& shaderParam : paramInfo.m_srvs )
			{
				if ( shaderParam.m_space >= 100 ) // Skip bindless
				{
					continue;
				}

				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				range.NumDescriptors = 1;
				range.BaseShaderRegister = shaderParam.m_bindPoint;
				range.RegisterSpace = shaderParam.m_space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = GetShaderVisibility( type );;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}

			for ( const ShaderParameter& shaderParam : paramInfo.m_uavs )
			{
				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				range.NumDescriptors = 1;
				range.BaseShaderRegister = shaderParam.m_bindPoint;
				range.RegisterSpace = shaderParam.m_space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = GetShaderVisibility( type );;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}

			for ( const ShaderParameter& shaderParam : paramInfo.m_constantBuffers )
			{
				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				range.NumDescriptors = 1;
				range.BaseShaderRegister = shaderParam.m_bindPoint;
				range.RegisterSpace = shaderParam.m_space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = GetShaderVisibility( type );;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}

			for ( const ShaderParameter& shaderParam : paramInfo.m_samplers )
			{
				if ( shaderParam.m_space == 100 ) // Skip bindless
				{
					continue;
				}

				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
				range.NumDescriptors = 1;
				range.BaseShaderRegister = shaderParam.m_bindPoint;
				range.RegisterSpace = shaderParam.m_space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = GetShaderVisibility( type );;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}
		}
	}
}
