#pragma once

#include "Buffer.h"
#include "Common.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace rendercore
{
	class ComputeShader;
	class PixelShader;
	class VertexShader;

	class ConstantBuffer
	{
	public:
		void Update( const void* data, uint32 size );
		void* Lock();
		void Unlock();

		size_t Size() const;

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

		ConstantBuffer( uint32 size );

		ConstantBuffer() = default;
		virtual ~ConstantBuffer() = default;
		ConstantBuffer( const ConstantBuffer& ) = default;
		ConstantBuffer& operator=( const ConstantBuffer& ) = default;
		ConstantBuffer( ConstantBuffer&& ) = default;
		ConstantBuffer& operator=( ConstantBuffer&& ) = default;

	protected:
		void InitResource( uint32 size );

	private:
		agl::RefHandle<agl::Buffer> m_buffer;
		uint32 m_size = 0;
	};
}
