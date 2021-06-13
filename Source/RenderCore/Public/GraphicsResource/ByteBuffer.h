#pragma once
#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Shader.h"

class UploadBuffer;

class DistributionCopyCS
{
public:
	DistributionCopyCS( );
	ComputeShader* Shader( ) { return m_shader; }

	static constexpr int ThreadGroupX = 64;

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
	GpuMemcpy( std::size_t numUpload, UINT sizePerFloat4, UploadBuffer& src, UploadBuffer& distributer );

	void Add( const char* data, unsigned int dstIndex );

	void Upload( aga::Buffer* destBuffer );

private:
	UploadBuffer& m_src;
	UploadBuffer& m_distributer;

	char* m_pUploadData = nullptr;
	UINT* m_pDistributionData = nullptr;

	UINT m_distributionCount = 0;

	UINT m_sizePerFloat4 = 0;
};