#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <array>
#include <memory>
#include <vector>

namespace agl
{
	class ShaderBindings;
	class ShaderParameter;
	class SingleShaderBindings;

	class GlobalConstantBuffer
	{
	public:
		void Reset();
		void SetShaderValue( const ShaderParameter& parameter, const void* value );
		
		virtual void Prepare() = 0;
		virtual void CommitShaderValue() = 0;
		virtual void AddGlobalConstantBuffer( const ShaderParameter& parameter, SingleShaderBindings& singleShaderBindings ) = 0;

		virtual ~GlobalConstantBuffer() = default;

	protected:
		static constexpr int MaxSize = 2048;
		std::array<char, MaxSize> m_data;
		uint32 m_updateSize = 0;
	};

	class GlobalSyncConstantBuffer : public GlobalConstantBuffer
	{
	public:
		virtual void Prepare() override;
		virtual void CommitShaderValue() override;
		virtual void AddGlobalConstantBuffer( const ShaderParameter& parameter, SingleShaderBindings& singleShaderBindings ) override;

	private:
		static constexpr int32 DefaultBufferSize = 2048;
		RefHandle<Buffer> m_buffer;
	};

	class GlobalAsyncConstantBuffer : public GlobalConstantBuffer
	{
	public:
		virtual void Prepare() override;
		virtual void CommitShaderValue() override;
		virtual void AddGlobalConstantBuffer( const ShaderParameter& parameter, SingleShaderBindings& singleShaderBindings ) override;

	private:
		std::vector<RefHandle<Buffer>> m_buffers;
	};

	class GlobalConstantBuffers
	{
	public:
		void Initialize();
		void Prepare();

		void Reset( bool bCompute );
		void SetShaderValue( const ShaderParameter& parameter, const void* value );
		void CommitShaderValue( bool bCompute );
		void AddGlobalConstantBuffers( ShaderBindings& shaderBindings );

		virtual ~GlobalConstantBuffers() = default;

	protected:
		virtual GlobalConstantBuffer* CreateGlobalConstantBuffer() const = 0;
		GlobalConstantBuffer& GetGlobalConstantBuffer( ShaderType shaderType );

		std::unique_ptr<GlobalConstantBuffer> m_constantBuffers[MAX_SHADER_TYPE<uint32>] = {};
	};

	class GlobalSyncConstantBuffers : public GlobalConstantBuffers
	{
	protected:
		virtual GlobalConstantBuffer* CreateGlobalConstantBuffer() const override;
	};

	class GlobalAsyncConstantBuffers : public GlobalConstantBuffers
	{
	protected:
		virtual GlobalConstantBuffer* CreateGlobalConstantBuffer() const override;
	};
}
