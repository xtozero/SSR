#include "GlobalConstantBuffers.h"

#include "IAgl.h"
#include "LibraryTool/InterfaceFactories.h"
#include "ShaderBindings.h"
#include "TaskScheduler.h"

#include <cassert>

namespace agl
{
	void GlobalConstantBuffer::Reset()
	{
		m_updateSize = 0;
	}

	void GlobalConstantBuffer::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		assert( parameter.m_offset + parameter.m_sizeInByte <= MaxSize );

		std::memcpy( m_data.data() + parameter.m_offset, value, parameter.m_sizeInByte );
		m_updateSize = std::max(m_updateSize, parameter.m_offset + parameter.m_sizeInByte );
	}

	void GlobalSyncConstantBuffer::Prepare()
	{
		// Do Nothing
	}

	void GlobalSyncConstantBuffer::CommitShaderValue()
	{
		if ( m_updateSize == 0 )
		{
			return;
		}

		assert( m_buffer.Get() != nullptr );
		void* dest = GetInterface<IAgl>()->Lock( m_buffer ).m_data;
		std::memcpy( dest, m_data.data(), m_updateSize );
		GetInterface<IAgl>()->UnLock( m_buffer );
	}

	void GlobalSyncConstantBuffer::AddGlobalConstantBuffer( const ShaderParameter& parameter, SingleShaderBindings& singleShaderBindings )
	{
		assert( IsInRenderThread() );

		if ( m_updateSize == 0 )
		{
			return;
		}

		if ( m_buffer == nullptr )
		{
			BUFFER_TRAIT trait = {
			   .m_stride = DefaultBufferSize,
			   .m_count = 1,
			   .m_access = ResourceAccessFlag::GpuRead | ResourceAccessFlag::CpuWrite,
			   .m_bindType = ResourceBindType::ConstantBuffer,
			   .m_miscFlag = ResourceMisc::None,
			   .m_format = ResourceFormat::Unknown
			};

			m_buffer = Buffer::Create( trait );
			m_buffer->Init();
		}

		singleShaderBindings.AddConstantBuffer( parameter, m_buffer );
	}

	void GlobalAsyncConstantBuffer::Prepare()
	{
		m_buffers.clear();
	}

	void GlobalAsyncConstantBuffer::CommitShaderValue()
	{
		if ( m_updateSize == 0 )
		{
			return;
		}

		assert( m_buffers.empty() == false );
		Buffer* buffer = m_buffers.back().Get();

		assert( buffer != nullptr );
		void* dest = GetInterface<IAgl>()->Lock( buffer ).m_data;
		std::memcpy( dest, m_data.data(), m_updateSize );
		GetInterface<IAgl>()->UnLock( buffer );
	}

	void GlobalAsyncConstantBuffer::AddGlobalConstantBuffer( const ShaderParameter& parameter, SingleShaderBindings& singleShaderBindings )
	{
		assert( IsInRenderThread() );

		if ( m_updateSize == 0 )
		{
			return;
		}

		BUFFER_TRAIT trait = {
			   .m_stride = m_updateSize,
			   .m_count = 1,
			   .m_access = ResourceAccessFlag::GpuRead | ResourceAccessFlag::CpuWrite,
			   .m_bindType = ResourceBindType::ConstantBuffer,
			   .m_miscFlag = ResourceMisc::None,
			   .m_format = ResourceFormat::Unknown
		};

		RefHandle<Buffer> buffer = Buffer::Create( trait );
		buffer->Init();

		singleShaderBindings.AddConstantBuffer( parameter, buffer );

		m_buffers.push_back( buffer );
	}

	void GlobalConstantBuffers::Initialize()
	{
		for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
		{
			m_constantBuffers[i].reset( CreateGlobalConstantBuffer() );
		}
	}

	void GlobalConstantBuffers::Prepare()
	{
		for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
		{
			GetGlobalConstantBuffer( static_cast<ShaderType>( i ) ).Prepare();
		}
	}

	void GlobalConstantBuffers::Reset( bool bCompute )
	{
		if ( bCompute )
		{
			GetGlobalConstantBuffer( ShaderType::CS ).Reset();
		}
		else
		{
			GetGlobalConstantBuffer( ShaderType::VS ).Reset();
			GetGlobalConstantBuffer( ShaderType::HS ).Reset();
			GetGlobalConstantBuffer( ShaderType::DS ).Reset();
			GetGlobalConstantBuffer( ShaderType::GS ).Reset();
			GetGlobalConstantBuffer( ShaderType::PS ).Reset();
		}
	}

	void GlobalConstantBuffers::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		GetGlobalConstantBuffer( parameter.m_shader ).SetShaderValue( parameter, value );
	}

	void GlobalConstantBuffers::CommitShaderValue( bool bCompute )
	{
		if ( bCompute )
		{
			GetGlobalConstantBuffer( ShaderType::CS ).CommitShaderValue();
		}
		else
		{
			GetGlobalConstantBuffer( ShaderType::VS ).CommitShaderValue();
			GetGlobalConstantBuffer( ShaderType::HS ).CommitShaderValue();
			GetGlobalConstantBuffer( ShaderType::DS ).CommitShaderValue();
			GetGlobalConstantBuffer( ShaderType::GS ).CommitShaderValue();
			GetGlobalConstantBuffer( ShaderType::PS ).CommitShaderValue();
		}
	}

	void GlobalConstantBuffers::AddGlobalConstantBuffers( ShaderBindings& shaderBindings )
	{
		for ( uint32 i = 0; i < MAX_SHADER_TYPE<uint32>; ++i )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( i ) );

			if ( binding.GetShaderType() == ShaderType::None )
			{
				continue;
			}

			const ShaderParameterInfo& info = binding.ParameterInfo();
			for ( const ShaderParameter& parameter : info.m_constantBuffers )
			{
				if ( parameter.m_bindPoint != 0 )
				{
					continue;
				}

				m_constantBuffers[i]->AddGlobalConstantBuffer( parameter, binding );
			}
		}
	}

	GlobalConstantBuffer& GlobalConstantBuffers::GetGlobalConstantBuffer( ShaderType shaderType )
	{
		assert( m_constantBuffers[static_cast<int>( shaderType )] );
		return *m_constantBuffers[static_cast<int>( shaderType )];
	}

	GlobalConstantBuffer* GlobalSyncConstantBuffers::CreateGlobalConstantBuffer() const
	{
		return new GlobalSyncConstantBuffer();
	}

	GlobalConstantBuffer* GlobalAsyncConstantBuffers::CreateGlobalConstantBuffer() const
	{
		return new GlobalAsyncConstantBuffer();
	}
}
