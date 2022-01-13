#pragma once

#include "IAga.h"
#include "GraphicsPipelineState.h"
#include "RenderOption.h"
#include "ShaderParameterMap.h"
#include "SizedTypes.h"
#include "VertexLayout.h"

#include <unordered_map>

class VertexShader;

class AbstractGraphicsInterface
{
public:
	void BootUp( aga::IAga* pAga );
	void Shutdown();

	void* Lock( aga::Buffer* buffer, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 );
	void UnLock( aga::Buffer* buffer, uint32 subResource = 0 );

	void Copy( aga::Buffer* dst, aga::Buffer* src, uint32 size );

	std::unique_ptr<aga::IDeferredCommandList> CreateDeferredCommandList() const;
	aga::IImmediateCommandList* GetImmediateCommandList();

	BlendState FindOrCreate( const BlendOption& option );
	DepthStencilState FindOrCreate( const DepthStencilOption& option );
	RasterizerState FindOrCreate( const RasterizerOption& option );
	SamplerState FindOrCreate( const SamplerOption& option );
	VertexLayout FindOrCreate( const VertexShader& vs, const VertexLayoutDesc& desc );

private:
	aga::IAga* m_aga = nullptr;

	std::unordered_map<BlendOption, BlendState, BlendOptionHasher> m_blendStates;
	std::unordered_map<DepthStencilOption, DepthStencilState, DepthStencilOptionHasher> m_depthStencilStates;
	std::unordered_map<RasterizerOption, RasterizerState, RasterizerOptionHasher> m_rasterizerStates;
	std::unordered_map<SamplerOption, SamplerState, SamplerOptionHasher> m_samplerStates;
	std::unordered_map<VertexLayoutDesc, VertexLayout, VertexLayoutDescHasher> m_vertexLayouts;
};

AbstractGraphicsInterface& GraphicsInterface();