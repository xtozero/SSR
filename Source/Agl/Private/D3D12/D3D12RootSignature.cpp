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
		case agl::ShaderType::Vertex:
			return D3D12_SHADER_VISIBILITY_VERTEX;
		case agl::ShaderType::Hull:
			return D3D12_SHADER_VISIBILITY_HULL;
		case agl::ShaderType::Domain:
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		case agl::ShaderType::Geometry:
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case agl::ShaderType::Pixel:
			return D3D12_SHADER_VISIBILITY_PIXEL;
		case agl::ShaderType::Compute:
			return D3D12_SHADER_VISIBILITY_ALL;
		case agl::ShaderType::Mesh:
			return D3D12_SHADER_VISIBILITY_MESH;
		case agl::ShaderType::Amplification:
			return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		default:
			break;
		}

		assert( false && "GetShaderVisibility - Unsurpported ShaderType" );
		return D3D12_SHADER_VISIBILITY_ALL;
	}

	ResourceStatistics SurveyShaderParamInfo( const ShaderParameterInfo& paramInfo )
	{
		return { static_cast<uint32>( paramInfo.m_srvs.size() )
			, static_cast<uint32>( paramInfo.m_uavs.size() )
			, static_cast<uint32>( paramInfo.m_constantBuffers.size() )
			, static_cast<uint32>( paramInfo.m_samplers.size() )
			, static_cast<uint32>( paramInfo.m_bindless.size() ) };
	}

	ResourceStatistics SurveyShader( const Shader& shader )
	{
		const ShaderParameterInfo& paramInfo = shader.GetParameterInfo();
		return SurveyShaderParamInfo( paramInfo );
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

		auto meshShader = static_cast<D3D12MeshShader*>( initializer.m_meshShader );
		if ( meshShader )
		{
			statistics += SurveyShader( *meshShader );
		}

		auto amplificationShader = static_cast<D3D12AmplificationShader*>( initializer.m_amplificationShader );
		if ( amplificationShader )
		{
			statistics += SurveyShader( *amplificationShader );
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
		ShaderParameterInfoArray paramInfoArray;

		if ( initializer.m_vertexShader )
		{
			paramInfoArray.emplace_back( ShaderType::Vertex, &initializer.m_vertexShader->GetParameterInfo() );
		}

		if ( initializer.m_geometryShader )
		{
			paramInfoArray.emplace_back( ShaderType::Geometry, &initializer.m_geometryShader->GetParameterInfo() );
		}

		if ( initializer.m_piexlShader )
		{
			paramInfoArray.emplace_back( ShaderType::Pixel, &initializer.m_piexlShader->GetParameterInfo() );
		}

		if ( initializer.m_meshShader )
		{
			paramInfoArray.emplace_back( ShaderType::Mesh, &initializer.m_meshShader->GetParameterInfo() );
		}

		if ( initializer.m_amplificationShader )
		{
			paramInfoArray.emplace_back( ShaderType::Amplification, &initializer.m_amplificationShader->GetParameterInfo() );
		}

		ResourceStatistics statistics = SurveyPipeline( initializer );

		bool hasBindless = statistics.NumBindless() > 0;
		m_parameters.reserve( hasBindless ? ( statistics.TotalBinding() + 2 ) : statistics.NumResourceCategory() );
		m_descritorRange.reserve( statistics.TotalBinding() + ( hasBindless ? 5 : 0 ) );

		if ( hasBindless )
		{
			InitializeForBindless( paramInfoArray );
		}
		else
		{
			for ( const auto& [type, paramInfo] : paramInfoArray )
			{
				InitializeSRV( type, *paramInfo );
			}

			for ( const auto& [type, paramInfo] : paramInfoArray )
			{
				InitializeUAV( type, *paramInfo );
			}

			for ( const auto& [type, paramInfo] : paramInfoArray )
			{
				InitializeCB( type, *paramInfo );
			}

			for ( const auto& [type, paramInfo] : paramInfoArray )
			{
				InitializeSampler( type, *paramInfo );
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

		ResourceStatistics statistics = SurveyPipeline( initializer );

		bool hasBindless = statistics.NumBindless() > 0;
		m_parameters.reserve( hasBindless ? ( statistics.TotalBinding() + 2 ) : statistics.NumResourceCategory() );
		m_descritorRange.reserve( statistics.TotalBinding() + ( hasBindless ? 5 : 0 ) );

		const ShaderParameterInfo& paramInfo = computeShader->GetParameterInfo();
		if ( hasBindless )
		{
			ShaderParameterInfoArray paramInfoArray;
			paramInfoArray.emplace_back( ShaderType::Compute, &paramInfo );

			InitializeForBindless( paramInfoArray );
		}
		else
		{
			InitializeSRV( ShaderType::Compute, paramInfo );
			InitializeUAV( ShaderType::Compute, paramInfo );
			InitializeCB( ShaderType::Compute, paramInfo );
			InitializeSampler( ShaderType::Compute, paramInfo );
		}

		m_desc.NumParameters = static_cast<uint32>( m_parameters.size() );
		m_desc.pParameters = m_parameters.data();
		m_desc.NumStaticSamplers = 0;
		m_desc.pStaticSamplers = nullptr;
		m_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	}

	D3D12RootSignature::D3D12RootSignature( const D3D12RaytracingShaderTable& shaderTable )
	{
		const ShaderParameterInfo& paramInfo = shaderTable.GetParameterInfo();
		ResourceStatistics statistics = SurveyShaderParamInfo( paramInfo );

		bool hasBindless = statistics.NumBindless() > 0;
		m_parameters.reserve( hasBindless ? ( statistics.TotalBinding() + 2 ) : statistics.NumResourceCategory() );
		m_descritorRange.reserve( statistics.TotalBinding() + ( hasBindless ? 5 : 0 ) );

		if ( hasBindless )
		{
			ShaderParameterInfoArray paramInfoArray;
			paramInfoArray.emplace_back( ShaderType::RayTracing, &paramInfo );

			InitializeForBindless( paramInfoArray );
		}
		else
		{
			InitializeSRV( ShaderType::RayTracing, paramInfo );
			InitializeUAV( ShaderType::RayTracing, paramInfo );
			InitializeCB( ShaderType::RayTracing, paramInfo );
			InitializeSampler( ShaderType::RayTracing, paramInfo );
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

	void D3D12RootSignature::InitializeForBindless( ShaderParameterInfoArray& paramInfoArray )
	{
		{
			constexpr int32 MaxStandardSrvCount = 4;
			for ( int32 i = 0; i < MaxStandardSrvCount; ++i )
			{
				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				range.NumDescriptors = static_cast<uint32>( -1 );
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
			range.NumDescriptors = static_cast<uint32>( -1 );
			range.BaseShaderRegister = 0;
			range.RegisterSpace = 100;
			range.OffsetInDescriptorsFromTableStart = 0;

			D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.DescriptorTable.NumDescriptorRanges = 1;
			param.DescriptorTable.pDescriptorRanges = &range;
		}

		for ( auto& [type, paramInfo] : paramInfoArray )
		{
			for ( const ShaderParameter& shaderParam : paramInfo->m_srvs )
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
				param.ShaderVisibility = GetShaderVisibility( type );
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}

			for ( const ShaderParameter& shaderParam : paramInfo->m_uavs )
			{
				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				range.NumDescriptors = 1;
				range.BaseShaderRegister = shaderParam.m_bindPoint;
				range.RegisterSpace = shaderParam.m_space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = GetShaderVisibility( type );
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}

			for ( const ShaderParameter& shaderParam : paramInfo->m_constantBuffers )
			{
				D3D12_DESCRIPTOR_RANGE& range = m_descritorRange.emplace_back();

				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				range.NumDescriptors = 1;
				range.BaseShaderRegister = shaderParam.m_bindPoint;
				range.RegisterSpace = shaderParam.m_space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				D3D12_ROOT_PARAMETER& param = m_parameters.emplace_back();

				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = GetShaderVisibility( type );
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}

			for ( const ShaderParameter& shaderParam : paramInfo->m_samplers )
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
				param.ShaderVisibility = GetShaderVisibility( type );
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &range;
			}
		}
	}
}
