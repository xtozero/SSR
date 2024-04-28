#include "Shader.h"

#include "ArchiveUtility.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void ShaderBase::SetHash( size_t hash )
	{
		m_hash = hash;

		if ( IsValid() )
		{
			m_shader->SetHash( hash );
		}
	}

	size_t ShaderBase::GetHash() const
	{
		return m_hash;
	}

	Archive& operator<<( Archive& ar, ShaderBase& shaderBase )
	{
		shaderBase.Serialize( ar );
		return ar;
	}

	StaticShaderSwitches ShaderBase::GetStaticSwitches() const
	{
		return StaticShaderSwitches();
	}

	ShaderBase* ShaderBase::CompileShader( [[maybe_unused]] const StaticShaderSwitches& switches )
	{
		return this;
	}

	void ShaderBase::PostLoadImpl()
	{
		CreateShader();
	}

	REGISTER_ASSET( VertexShader );
	agl::VertexShader* VertexShader::Resource()
	{
		return static_cast<agl::VertexShader*>( m_shader.Get() );
	}

	const agl::VertexShader* VertexShader::Resource() const
	{
		return static_cast<agl::VertexShader*>( m_shader.Get() );
	}

	void VertexShader::CreateShader()
	{
		m_shader = agl::VertexShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( GeometryShader );
	agl::GeometryShader* GeometryShader::Resource()
	{
		return static_cast<agl::GeometryShader*>( m_shader.Get() );
	}

	const agl::GeometryShader* GeometryShader::Resource() const
	{
		return  static_cast<agl::GeometryShader*>( m_shader.Get() );
	}

	void GeometryShader::CreateShader()
	{
		m_shader = agl::GeometryShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}


	REGISTER_ASSET( PixelShader );
	agl::PixelShader* PixelShader::Resource()
	{
		return static_cast<agl::PixelShader*>( m_shader.Get() );
	}

	const agl::PixelShader* PixelShader::Resource() const
	{
		return static_cast<agl::PixelShader*>( m_shader.Get() );
	}

	void PixelShader::CreateShader()
	{
		m_shader = agl::PixelShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( ComputeShader );
	agl::ComputeShader* ComputeShader::Resource()
	{
		return static_cast<agl::ComputeShader*>( m_shader.Get() );
	}

	const agl::ComputeShader* ComputeShader::Resource() const
	{
		return static_cast<agl::ComputeShader*>( m_shader.Get() );
	}

	void ComputeShader::CreateShader()
	{
		m_shader = agl::ComputeShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}
}
