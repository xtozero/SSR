#pragma once

#include "GraphicsApiResource.h"
#include "Memory/InlineMemoryAllocator.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
	class ComputePipelineStateDesc;
	class D3D12RaytracingShaderTable;
	class GraphicsPipelineStateDesc;
	class Shader;
	class ShaderParameterInfo;

	class ResourceStatistics
	{
	public:
		uint32 NumSRV() const
		{
			return m_numSRV;
		}

		uint32 NumUAV() const
		{
			return m_numUAV;
		}

		uint32 NumCB() const
		{
			return m_numCB;
		}

		uint32 NumSampler() const
		{
			return m_numSampler;
		}

		uint32 NumBindless() const
		{
			return m_numBindless;
		}

		uint32 TotalBinding() const
		{
			return NumSRV() + NumUAV() + NumCB() + NumSampler();
		}

		uint32 NumResourceCategory() const
		{
			uint32 numResourceCategory = 0;
			if ( NumSRV() > 0 )
			{
				++numResourceCategory;
			}
			if ( NumUAV() > 0 )
			{
				++numResourceCategory;
			}
			if ( NumCB() > 0 )
			{
				++numResourceCategory;
			}
			if ( NumSampler() > 0 )
			{
				++numResourceCategory;
			}

			return numResourceCategory;
		}

		ResourceStatistics() = default;
		ResourceStatistics( uint32 numSRV, uint32 numUAV, uint32 numCB, uint32 numSampler, uint32 hasBindless ) noexcept
			: m_numSRV( numSRV )
			, m_numUAV( numUAV )
			, m_numCB( numCB )
			, m_numSampler( numSampler )
			, m_numBindless( hasBindless )
		{}

		ResourceStatistics& operator+=( const ResourceStatistics& other )
		{
			m_numSRV += other.m_numSRV;
			m_numUAV += other.m_numUAV;
			m_numCB += other.m_numCB;
			m_numSampler += other.m_numSampler;
			m_numBindless += other.m_numBindless;

			return *this;
		}

	private:
		uint32 m_numSRV = 0;
		uint32 m_numUAV = 0;
		uint32 m_numCB = 0;
		uint32 m_numSampler = 0;
		uint32 m_numBindless = false;
	};

	class D3D12RootSignature final : public GraphicsApiResource
	{
	public:
		ID3D12RootSignature* Resource() const;

		explicit D3D12RootSignature( const GraphicsPipelineStateDesc& desc );
		explicit D3D12RootSignature( const ComputePipelineStateDesc& desc );
		explicit D3D12RootSignature( const D3D12RaytracingShaderTable& shaderTable );

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		void InitializeSRV( ShaderType shaderType, const ShaderParameterInfo& paramInfo );
		void InitializeUAV( ShaderType shaderType, const ShaderParameterInfo& paramInfo );
		void InitializeCB( ShaderType shaderType, const ShaderParameterInfo& paramInfo );
		void InitializeSampler( ShaderType shaderType, const ShaderParameterInfo& paramInfo );

		using ShaderParameterInfoArray = std::vector<std::pair<ShaderType, const ShaderParameterInfo*>, InlineAllocator<std::pair<ShaderType, const ShaderParameterInfo*>, NumShaderTypes<uint32>>>;
		void InitializeForBindless( ShaderParameterInfoArray& paramInfoArray );

		ID3D12RootSignature* m_rootSignature = nullptr;
		D3D12_ROOT_SIGNATURE_DESC m_desc = {};
		std::vector<D3D12_ROOT_PARAMETER> m_parameters;
		std::vector<D3D12_DESCRIPTOR_RANGE> m_descritorRange;
	};
}
