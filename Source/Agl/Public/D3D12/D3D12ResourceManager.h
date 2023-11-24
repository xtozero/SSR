#pragma once

#include "GuideTypes.h"
#include "HashUtil.h"
#include "IRenderResourceManager.h"

#include <d3d12.h>
#include <map>
#include <shared_mutex>
#include <unordered_map>
#include <wrl/client.h>

namespace agl
{
	class D3D12ResourceManager final : public IResourceManager
	{
	public:
		virtual void Shutdown() override;

		// Texture
		virtual Texture* CreateTexture( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData ) override;

		// Buffer
		virtual Buffer* CreateBuffer( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData ) override;

		// Shader
		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size ) override;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;
		virtual VertexShader* CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;
		virtual GeometryShader* CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;
		virtual PixelShader* CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;

		// RenderState
		virtual BlendState* CreateBlendState( const BlendStateTrait& trait ) override;
		virtual DepthStencilState* CreateDepthStencilState( const DepthStencilStateTrait& trait ) override;
		virtual RasterizerState* CreateRasterizerState( const RasterizerStateTrait& trait ) override;
		virtual SamplerState* CreateSamplerState( const SamplerStateTrait& trait ) override;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) override;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) override;

		// Canvas
		virtual Canvas* CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format ) override;

		// Viewport
		virtual Viewport* CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) override;
		virtual Viewport* CreateViewport( Canvas& canvas ) override;

		ID3D12PipelineState* FindOrCreate( GraphicsPipelineState* pipelineState, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc );

		D3D12ResourceManager() = default;
		virtual ~D3D12ResourceManager() override;
		D3D12ResourceManager( const D3D12ResourceManager& ) = delete;
		D3D12ResourceManager( D3D12ResourceManager&& ) = delete;
		D3D12ResourceManager& operator=( const D3D12ResourceManager& ) = delete;
		D3D12ResourceManager& operator=( D3D12ResourceManager&& ) = delete;

	private:
		std::map<GraphicsPipelineStateInitializer, RefHandle<GraphicsPipelineState>> m_graphicsPipelineStateCache;
		std::map<ComputePipelineStateInitializer, RefHandle<ComputePipelineState>> m_computePipelineStateCache;

		struct D3D12PipelineStateKey
		{
			GraphicsPipelineState* m_state;
			DXGI_FORMAT m_outputFormats[9] = {};

			D3D12PipelineStateKey( GraphicsPipelineState* state, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc )
				: m_state( state )
				, m_outputFormats{ 
					desc.RTVFormats[0], 
					desc.RTVFormats[1],
					desc.RTVFormats[2],
					desc.RTVFormats[3],
					desc.RTVFormats[4],
					desc.RTVFormats[5],
					desc.RTVFormats[6],
					desc.RTVFormats[7],
					desc.DSVFormat }
			{}
			D3D12PipelineStateKey() = default;

			friend bool operator==( const D3D12PipelineStateKey& lhs, const D3D12PipelineStateKey& rhs )
			{
				return lhs.m_state == rhs.m_state
					&& std::equal( std::begin( lhs.m_outputFormats ), std::end( lhs.m_outputFormats ), std::begin( rhs.m_outputFormats ) );
			}
		};

		struct D3D12PipelineStateKeyHasher
		{
			size_t operator()( const D3D12PipelineStateKey& key ) const
			{
				static size_t typeHash = typeid( D3D12PipelineStateKeyHasher ).hash_code();
				size_t hash = typeHash;

				HashCombine( hash, key.m_state );

				for ( DXGI_FORMAT format : key.m_outputFormats )
				{
					HashCombine( hash, format );
				}

				return hash;
			}
		};

		std::shared_mutex m_d3d12PipelineMutex;
		std::unordered_map<D3D12PipelineStateKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>, D3D12PipelineStateKeyHasher> m_d3d12PipelineState;
	};

	Owner<IResourceManager*> CreateD3D12ResourceManager();
}
