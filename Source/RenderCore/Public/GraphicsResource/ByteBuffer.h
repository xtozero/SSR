#pragma once

#include "Buffer.h"
#include "GlobalShaders.h"
#include "GraphicsApiResource.h"
#include "Shader.h"
#include "ShaderParameterUtils.h"
#include "SizedTypes.h"

namespace rendercore
{
	class UploadBuffer;

	class DistributionCopyCS : public GlobalShaderCommon<ComputeShader, DistributionCopyCS>
	{
	public:
		static constexpr uint32 ThreadGroupX = 64;

		DEFINE_SHADER_PARAM( NumDistribution );
		DEFINE_SHADER_PARAM( Src );
		DEFINE_SHADER_PARAM( Distributer );
		DEFINE_SHADER_PARAM( Dest );
	};

	class GpuMemcpy
	{
	public:
		GpuMemcpy( uint32 numUpload, uint32 sizePerFloat4, UploadBuffer& src, UploadBuffer& distributer );

		void Add( const char* data, uint32 dstIndex );

		void Upload( agl::Buffer* destBuffer );

	private:
		UploadBuffer& m_src;
		UploadBuffer& m_distributer;

		char* m_pUploadData = nullptr;
		uint32* m_pDistributionData = nullptr;

		uint32 m_distributionCount = 0;

		uint32 m_sizePerFloat4 = 0;
	};
}
