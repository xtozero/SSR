#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
	class ComputePipelineStateInitializer;
	class GraphicsPipelineStateInitializer;
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

		uint32 Total() const
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
		ResourceStatistics( uint32 numSRV, uint32 numUAV, uint32 numCB, uint32 numSampler ) noexcept
			: m_numSRV( numSRV )
			, m_numUAV( numUAV )
			, m_numCB( numCB )
			, m_numSampler( numSampler )
		{}

		ResourceStatistics& operator+=( const ResourceStatistics& other )
		{
			m_numSRV += other.m_numSRV;
			m_numUAV += other.m_numUAV;
			m_numCB += other.m_numCB;
			m_numSampler += other.m_numSampler;

			return *this;
		}

	private:
		uint32 m_numSRV = 0;
		uint32 m_numUAV = 0;
		uint32 m_numCB = 0;
		uint32 m_numSampler = 0;
	};

	class D3D12RootSignature : public DeviceDependantResource
	{
	public:
		ID3D12RootSignature* Resource() const;

		explicit D3D12RootSignature( const GraphicsPipelineStateInitializer& initializer );
		explicit D3D12RootSignature( const ComputePipelineStateInitializer& initializer );

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		void InitializeSRV( ShaderType shaderType, const ShaderParameterInfo& paramInfo );
		void InitializeUAV( ShaderType shaderType, const ShaderParameterInfo& paramInfo );
		void InitializeCB( ShaderType shaderType, const ShaderParameterInfo& paramInfo );
		void InitializeSampler( ShaderType shaderType, const ShaderParameterInfo& paramInfo );

		ID3D12RootSignature* m_rootSignature = nullptr;
		D3D12_ROOT_SIGNATURE_DESC m_desc = {};
		std::vector<D3D12_ROOT_PARAMETER> m_parameters;
		std::vector<D3D12_DESCRIPTOR_RANGE> m_descritorRange;
	};
}
