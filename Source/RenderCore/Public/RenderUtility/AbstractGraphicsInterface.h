#pragma once

#include "IAgl.h"
#include "GraphicsPipelineState.h"
#include "RenderOption.h"
#include "ShaderParameterMap.h"
#include "SizedTypes.h"
#include "VertexLayout.h"

#include <unordered_map>

class VertexShader;

namespace rendercore
{
	class AbstractGraphicsInterface final
	{
	public:
		void BootUp( agl::IAgl* pAgl );
		void Shutdown();

		agl::LockedResource Lock( agl::Buffer* buffer, agl::ResourceLockFlag lockFlag = agl::ResourceLockFlag::WriteDiscard, uint32 subResource = 0 );
		void UnLock( agl::Buffer* buffer, uint32 subResource = 0 );

		agl::ICommandList* GetCommandList();
		agl::IParallelCommandList* GetParallelCommandList();

		BlendState FindOrCreate( const BlendOption& option );
		DepthStencilState FindOrCreate( const DepthStencilOption& option );
		RasterizerState FindOrCreate( const RasterizerOption& option );
		SamplerState FindOrCreate( const SamplerOption& option );
		VertexLayout FindOrCreate( const VertexShader& vs, const VertexLayoutDesc& desc );

		BinaryChunk CompieShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const;
		bool BuildShaderMetaData( const BinaryChunk& byteCode, agl::ShaderParameterMap& outParameterMap, agl::ShaderParameterInfo& outParameterInfo ) const;

	private:
		agl::IAgl* m_agl = nullptr;

		std::unordered_map<BlendOption, BlendState, BlendOptionHasher> m_blendStates;
		std::unordered_map<DepthStencilOption, DepthStencilState, DepthStencilOptionHasher> m_depthStencilStates;
		std::unordered_map<RasterizerOption, RasterizerState, RasterizerOptionHasher> m_rasterizerStates;
		std::unordered_map<SamplerOption, SamplerState, SamplerOptionHasher> m_samplerStates;
		std::unordered_map<VertexLayoutInstance, VertexLayout, VertexLayoutInstanceHasher, VertexLayoutInstanceEqual> m_vertexLayouts;
	};

	AbstractGraphicsInterface& GraphicsInterface();
}
