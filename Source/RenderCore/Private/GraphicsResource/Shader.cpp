#include "stdafx.h"
#include "Shader.h"

#include "ArchiveUtility.h"
#include "TaskScheduler.h"

REGISTER_ASSET( VertexShader );
aga::VertexShader* VertexShader::Resource()
{
	return m_shader.Get();
}

const aga::VertexShader* VertexShader::Resource() const
{
	return m_shader.Get();
}

void VertexShader::PostLoadImpl()
{
	m_shader = aga::VertexShader::Create( m_byteCode.Data(), m_byteCode.Size() );
	EnqueueRenderTask(
		[shader = m_shader]()
		{
			shader->Init();
		} );
}

REGISTER_ASSET( GeometryShader );
aga::GeometryShader* GeometryShader::Resource()
{
	return m_shader.Get();
}

const aga::GeometryShader* GeometryShader::Resource() const
{
	return m_shader.Get();
}

RENDERCORE_DLL void GeometryShader::PostLoadImpl()
{
	m_shader = aga::GeometryShader::Create( m_byteCode.Data(), m_byteCode.Size() );
	EnqueueRenderTask(
		[shader = m_shader]()
		{
			shader->Init();
		} );
}


REGISTER_ASSET( PixelShader );
aga::PixelShader* PixelShader::Resource()
{
	return m_shader.Get();
}

const aga::PixelShader* PixelShader::Resource() const
{
	return m_shader.Get();
}

void PixelShader::PostLoadImpl()
{
	m_shader = aga::PixelShader::Create( m_byteCode.Data(), m_byteCode.Size() );
	EnqueueRenderTask(
		[shader = m_shader]()
		{
			shader->Init();
		} );
}

REGISTER_ASSET( ComputeShader );
aga::ComputeShader* ComputeShader::Resource()
{
	return m_shader.Get();
}

const aga::ComputeShader* ComputeShader::Resource() const
{
	return m_shader.Get();
}

void ComputeShader::PostLoadImpl()
{
	m_shader = aga::ComputeShader::Create( m_byteCode.Data(), m_byteCode.Size() );
	EnqueueRenderTask(
		[shader = m_shader]()
		{
			shader->Init();
		} );
}
