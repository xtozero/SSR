#include "stdafx.h"
#include "Shader.h"

#include "ArchiveUtility.h"
#include "TaskScheduler.h"

namespace rendercore
{
	Archive& operator<<( Archive& ar, ShaderBase& shaderBase )
	{
		shaderBase.Serialize( ar );
		return ar;
	}

	ShaderBase* ShaderBase::CompileShader( [[maybe_unused]] const StaticShaderSwitches& switches )
	{
		return this;
	}

	RENDERCORE_DLL void ShaderBase::PostLoadImpl()
	{
		CreateShader();
	}

	REGISTER_ASSET( VertexShader );
	agl::VertexShader* VertexShader::Resource()
	{
		return m_shader.Get();
	}

	const agl::VertexShader* VertexShader::Resource() const
	{
		return m_shader.Get();
	}

	void VertexShader::CreateShader()
	{
		m_shader = agl::VertexShader::Create( m_byteCode.Data(), m_byteCode.Size() );
		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( GeometryShader );
	agl::GeometryShader* GeometryShader::Resource()
	{
		return m_shader.Get();
	}

	const agl::GeometryShader* GeometryShader::Resource() const
	{
		return m_shader.Get();
	}

	void GeometryShader::CreateShader()
	{
		m_shader = agl::GeometryShader::Create( m_byteCode.Data(), m_byteCode.Size() );
		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}


	REGISTER_ASSET( PixelShader );
	agl::PixelShader* PixelShader::Resource()
	{
		return m_shader.Get();
	}

	const agl::PixelShader* PixelShader::Resource() const
	{
		return m_shader.Get();
	}

	void PixelShader::CreateShader()
	{
		m_shader = agl::PixelShader::Create( m_byteCode.Data(), m_byteCode.Size() );
		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( ComputeShader );
	agl::ComputeShader* ComputeShader::Resource()
	{
		return m_shader.Get();
	}

	const agl::ComputeShader* ComputeShader::Resource() const
	{
		return m_shader.Get();
	}

	void ComputeShader::CreateShader()
	{
		m_shader = agl::ComputeShader::Create( m_byteCode.Data(), m_byteCode.Size() );
		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}
}
