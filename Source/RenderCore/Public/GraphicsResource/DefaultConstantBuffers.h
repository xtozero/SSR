#pragma once

#include "ConstantBuffer.h"
#include "SizedTypes.h"

#include <type_traits>

namespace rendercore
{
	class ComputeShader;
	class PixelShader;
	class VertexShader;

	class DefaultConstantBuffers
	{
	public:
		static DefaultConstantBuffers& GetInstance()
		{
			static DefaultConstantBuffers defaultConstantBuffers;
			return defaultConstantBuffers;
		}

		void BootUp();
		void Shutdown();

		void SetShaderValue( agl::ShaderType shader, uint32 offset, uint32 numBytes, const void* value );
		void Commit( agl::ShaderType shader );
		void CommitAll();

		DefaultConstantBuffers() = default;
		DefaultConstantBuffers( const DefaultConstantBuffers& ) = delete;
		DefaultConstantBuffers& operator=( const DefaultConstantBuffers& ) = delete;
		DefaultConstantBuffers( DefaultConstantBuffers&& ) = delete;
		DefaultConstantBuffers& operator=( DefaultConstantBuffers&& ) = delete;
		~DefaultConstantBuffers()
		{
			Shutdown();
		}

	private:
		void SetShaderValueInternal( uint32 ctxIndex, uint32 offset, uint32 numBytes, const void* value );
		void CommitInternal( uint32 ctxIndex );

		static constexpr uint32 BUFFER_SIZE = 2048;

		struct ConstantBufferContext
		{
			ConstantBuffer m_buffer;
			unsigned char* m_dataStorage = nullptr;
			uint32 m_invalidRangeEnd = 0;
		} m_contexts[agl::MAX_SHADER_TYPE<uint32>];
	};
}
