#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Shader.h"
#include "SizedTypes.h"

class UploadBuffer;

class DistributionCopyCS
{
public:
	DistributionCopyCS( );
	ComputeShader* Shader( ) { return m_shader; }

	static constexpr uint32 ThreadGroupX = 64;

	aga::ShaderParameter m_numDistribution;
	aga::ShaderParameter m_src;
	aga::ShaderParameter m_distributer;
	aga::ShaderParameter m_dest;

private:
	ComputeShader* m_shader = nullptr;
};

class GpuMemcpy
{
public:
	GpuMemcpy( uint32 numUpload, uint32 sizePerFloat4, UploadBuffer& src, UploadBuffer& distributer );

	void Add( const char* data, uint32 dstIndex );

	void Upload( aga::Buffer* destBuffer );

private:
	UploadBuffer& m_src;
	UploadBuffer& m_distributer;

	char* m_pUploadData = nullptr;
	uint32* m_pDistributionData = nullptr;

	uint32 m_distributionCount = 0;

	uint32 m_sizePerFloat4 = 0;
};