#pragma once
#include "ConstantBuffer.h"

#include <type_traits>

class ComputeShader;
class IAga;
class PixelShader;
class VertexShader;

class DefaultConstantBuffers
{
public:
	void BootUp( );
	void Shutdown( );

	template <typename ShaderType>
	void SetShaderValue( [[maybe_unused]] ShaderType& shader, UINT offset, UINT numBytes, const void* value )
	{
		if constexpr ( std::is_same_v<VertexShader, ShaderType> )
		{
			SetShaderValue( VS, offset, numBytes, value );
		}
		else if constexpr ( std::is_same_v<PixelShader, ShaderType> )
		{
			SetShaderValue( PS, offset, numBytes, value );
		}
		else
		{
			SetShaderValue( CS, offset, numBytes, value );
		}
	}

	template <typename ShaderType>
	void Commit( [[maybe_unused]] ShaderType& shader )
	{
		if constexpr ( std::is_same_v<VertexShader, ShaderType> )
		{
			Commit( VS );
		}
		else if constexpr ( std::is_same_v<PixelShader, ShaderType> )
		{
			Commit( PS );
		}
		else
		{
			Commit( CS );
		}
	}

	void CommitAll( );

	DefaultConstantBuffers( ) = default;
	DefaultConstantBuffers( const DefaultConstantBuffers& ) = delete;
	DefaultConstantBuffers& operator=( const DefaultConstantBuffers& ) = delete;
	DefaultConstantBuffers( DefaultConstantBuffers&& ) = delete;
	DefaultConstantBuffers& operator=( DefaultConstantBuffers&& ) = delete;
	~DefaultConstantBuffers( )
	{
		Shutdown( );
	}

private:
	void SetShaderValue( int ctxIndex, UINT offset, UINT numBytes, const void* value );
	void Commit( int ctxIndex );

	enum
	{
		VS = 0,
		PS,
		CS,
		TotalConstants
	};

	static constexpr int BUFFER_SIZE = 2048;

	struct ConstantBufferContext
	{
		ConstantBuffer m_buffer;
		unsigned char* m_dataStorage = nullptr;
		std::size_t m_invalidRangeEnd = 0;
	} m_contexts[TotalConstants];
};