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
		case ShaderType::Vertex:
			return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderType::Hull:
			return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderType::Domain:
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		case ShaderType::Geometry:
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderType::Pixel:
			return D3D12_SHADER_VISIBILITY_PIXEL;
		case ShaderType::Compute:
			return D3D12_SHADER_VISIBILITY_ALL;
		case ShaderType::Mesh:
			return D3D12_SHADER_VISIBILITY_MESH;
		case ShaderType::Amplification:
			return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		case ShaderType::RayGen:
			[[fallthrough]];
		case ShaderType::Intersection:
			[[fallthrough]];
		case ShaderType::AnyHit:
			[[fallthrough]];
		case ShaderType::ClosestHit:
			[[fallthrough]];
		case ShaderType::Miss:
			[[fallthrough]];
		case ShaderType::Callable:
			return D3D12_SHADER_VISIBILITY_ALL;
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

	ResourceStatistics SurveyPipeline( const GraphicsPipelineStateDesc& desc )
	{
		ResourceStatistics statistics;

		auto vertexShader = static_cast<D3D12VertexShader*>( desc.m_vertexShader );
		if ( vertexShader )
		{
			statistics += SurveyShader( *vertexShader );
		}

		auto geometryShader = static_cast<D3D12GeometryShader*>( desc.m_geometryShader );
		if ( geometryShader )
		{
			statistics += SurveyShader( *geometryShader );
		}

		auto pixelShader = static_cast<D3D12PixelShader*>( desc.m_piexlShader );
		if ( pixelShader )
		{
			statistics += SurveyShader( *pixelShader );
		}

		auto meshShader = static_cast<D3D12MeshShader*>( desc.m_meshShader );
		if ( meshShader )
		{
			statistics += SurveyShader( *meshShader );
		}

		auto amplificationShader = static_cast<D3D12AmplificationShader*>( desc.m_amplificationShader );
		if ( amplificationShader )
		{
			statistics += SurveyShader( *amplificationShader );
		}

		return statistics;
	}

	ResourceStatistics SurveyPipeline( const ComputePipelineStateDesc& desc )
	{
		auto computeShader = static_cast<D3D12ComputeShader*>( desc.m_computeShader );
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

	D3D12RootSignature::D3D12RootSignature( const GraphicsPipelineStateDesc& desc )
	{
		ShaderParameterInfoArray paramInfoArray;

		if ( desc.m_vertexShader )
		{
			paramInfoArray.emplace_back( ShaderType::Vertex, &desc.m_vertexShader->GetParameterInfo() );
		}

		if ( desc.m_geometryShader )
		{
			paramInfoArray.emplace_back( ShaderType::Geometry, &desc.m_geometryShader->GetParameterInfo() );
		}

		if ( desc.m_piexlShader )
		{
			paramInfoArray.emplace_back( ShaderType::Pixel, &desc.m_piexlShader->GetParameterInfo() );
		}

		if ( desc.m_meshShader )
		{
			paramInfoArray.emplace_back( ShaderType::Mesh, &desc.m_meshShader->GetParameterInfo() );
		}

		if ( desc.m_amplificationShader )
		{
			paramInfoArray.emplace_back( ShaderType::Amplification, &desc.m_amplificationShader->GetParameterInfo() );
		}

		ResourceStatistics statistics = SurveyPipeline( desc );

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

		m_d3dDesc.NumParameters = static_cast<uint32>( m_parameters.size() );
		m_d3dDesc.pParameters = m_parameters.data();
		m_d3dDesc.NumStaticSamplers = 0;
		m_d3dDesc.pStaticSamplers = nullptr;
		m_d3dDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}

	D3D12RootSignature::D3D12RootSignature( const ComputePipelineStateDesc& desc )
	{
		auto computeShader = static_cast<D3D12ComputeShader*>( desc.m_computeShader );
		if ( computeShader == nullptr )
		{
			return;
		}

		ResourceStatistics statistics = SurveyPipeline( desc );

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

		m_d3dDesc.NumParameters = static_cast<uint32>( m_parameters.size() );
		m_d3dDesc.pParameters = m_parameters.data();
		m_d3dDesc.NumStaticSamplers = 0;
		m_d3dDesc.pStaticSamplers = nullptr;
		m_d3dDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	}

	D3D12RootSignature::D3D12RootSignature( const D3D12RaytracingShaderTable& shaderTable )
	{
		const ShaderParameterInfo& paramInfo = shaderTable.GetParameterInfo();
		ResourceStatistics statistics = SurveyShaderParamInfo( paramInfo );

		bool hasBindless = statistics.NumBindless() > 0;
		m_parameters.reserve( hasBindless ? ( statistics.TotalBinding() + 2 ) : statistics.NumResourceCategory() );
		m_descritorRange.reserve( statistics.TotalBinding() + ( hasBindless ? 5 : 0 ) );

		constexpr ShaderType RayTracingBindingStage = ShaderType::Compute;
		if ( hasBindless )
		{
			ShaderParameterInfoArray paramInfoArray;
			paramInfoArray.emplace_back( RayTracingBindingStage, &paramInfo );

			InitializeForBindless( paramInfoArray );
		}
		else
		{
			InitializeSRV( RayTracingBindingStage, paramInfo );
			InitializeUAV( RayTracingBindingStage, paramInfo );
			InitializeCB( RayTracingBindingStage, paramInfo );
			InitializeSampler( RayTracingBindingStage, paramInfo );
		}

		m_d3dDesc.NumParameters = static_cast<uint32>( m_parameters.size() );
		m_d3dDesc.pParameters = m_parameters.data();
		m_d3dDesc.NumStaticSamplers = 0;
		m_d3dDesc.pStaticSamplers = nullptr;
		m_d3dDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	}

	void D3D12RootSignature::InitResource()
	{
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		HRESULT hr = D3D12SerializeRootSignature( &m_d3dDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.GetAddressOf(), error.GetAddressOf() );
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
