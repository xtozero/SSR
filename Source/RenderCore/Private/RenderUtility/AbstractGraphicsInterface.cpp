#include "stdafx.h"
#include "AbstractGraphicsInterface.h"

#include "Shader.h"

namespace rendercore
{
	AbstractGraphicsInterface g_abstractGraphicsInterface;

	void AbstractGraphicsInterface::BootUp( agl::IAgl* pAgl )
	{
		m_agl = pAgl;
	}

	void AbstractGraphicsInterface::Shutdown()
	{
		m_agl = nullptr;
		m_blendStates.clear();
		m_depthStencilStates.clear();
		m_rasterizerStates.clear();
		m_samplerStates.clear();
		m_vertexLayouts.clear();
	}

	agl::LockedResource AbstractGraphicsInterface::Lock( agl::Buffer* buffer, agl::ResourceLockFlag lockFlag, uint32 subResource )
	{
		return m_agl->Lock( buffer, lockFlag, subResource );
	}

	agl::LockedResource AbstractGraphicsInterface::Lock( agl::Texture* texture, agl::ResourceLockFlag lockFlag, uint32 subResource )
	{
		return m_agl->Lock( texture, lockFlag, subResource );
	}

	void AbstractGraphicsInterface::UnLock( agl::Buffer* buffer, uint32 subResource )
	{
		m_agl->UnLock( buffer, subResource );
	}

	void AbstractGraphicsInterface::UnLock( agl::Texture* texture, uint32 subResource )
	{
		m_agl->UnLock( texture, subResource );
	}

	void AbstractGraphicsInterface::Copy( agl::Buffer* dst, agl::Buffer* src, uint32 size )
	{
		m_agl->Copy( dst, src, size );
	}

	agl::ICommandList* AbstractGraphicsInterface::GetCommandList()
	{
		return m_agl->GetCommandList();
	}

	agl::IParallelCommandList* AbstractGraphicsInterface::GetParallelCommandList()
	{
		return m_agl->GetParallelCommandList();
	}

	BlendState AbstractGraphicsInterface::FindOrCreate( const BlendOption& option )
	{
		auto found = m_blendStates.find( option );
		if ( found == m_blendStates.end() )
		{
			agl::BlendStateTrait trait;
			trait.m_alphaToConverageEnable = option.m_alphaToConverageEnable;
			trait.m_independentBlendEnable = option.m_independentBlendEnable;

			constexpr uint32 size = std::extent_v<decltype( option.m_renderTarget )>;
			for ( uint32 i = 0; i < size; ++i )
			{
				auto& dst = trait.m_renderTarget[i];
				auto& src = option.m_renderTarget[i];

				dst.m_blendEnable = src.m_blendEnable;
				dst.m_srcBlend = src.m_srcBlend;
				dst.m_destBlend = src.m_destBlend;
				dst.m_blendOp = src.m_blendOp;
				dst.m_srcBlendAlpha = src.m_srcBlendAlpha;
				dst.m_destBlendAlpha = src.m_destBlendAlpha;
				dst.m_blendOpAlpha = src.m_blendOpAlpha;
				dst.m_renderTargetWriteMask = src.m_renderTargetWriteMask;
			}

			trait.m_sampleMask = option.m_sampleMask;

			BlendState state( trait );
			m_blendStates.emplace( option, state );

			return state;
		}

		return found->second;
	}

	DepthStencilState AbstractGraphicsInterface::FindOrCreate( const DepthStencilOption& option )
	{
		auto found = m_depthStencilStates.find( option );
		if ( found == m_depthStencilStates.end() )
		{
			agl::DepthStencilStateTrait trait = {
				option.m_depth.m_enable,
				option.m_depth.m_writeDepth ? agl::DepthWriteMode::All : agl::DepthWriteMode::Zero,
				option.m_depth.m_depthFunc,
				option.m_stencil.m_enable,
				option.m_stencil.m_readMask,
				option.m_stencil.m_writeMask,
				option.m_stencil.m_frontFace,
				option.m_stencil.m_backFace
			};

			DepthStencilState state( trait );
			m_depthStencilStates.emplace( option, state );

			return state;
		}

		return found->second;
	}

	RasterizerState AbstractGraphicsInterface::FindOrCreate( const RasterizerOption& option )
	{
		auto found = m_rasterizerStates.find( option );
		if ( found == m_rasterizerStates.end() )
		{
			agl::RASTERIZER_STATE_TRAIT trait = {
				option.m_isWireframe ? agl::FillMode::Wireframe : agl::FillMode::Solid,
				option.m_cullMode,
				option.m_counterClockwise,
				option.m_depthBias,
				0.f,
				0.f,
				option.m_depthClipEnable,
				option.m_scissorEnable,
				option.m_multisampleEnable,
				option.m_antialiasedLineEnable
			};

			RasterizerState state( trait );
			m_rasterizerStates.emplace( option, state );

			return state;
		}

		return found->second;
	}

	SamplerState AbstractGraphicsInterface::FindOrCreate( const SamplerOption& option )
	{
		auto found = m_samplerStates.find( option );
		if ( found == m_samplerStates.end() )
		{
			agl::SAMPLER_STATE_TRAIT trait = {
				option.m_filter,
				option.m_addressU,
				option.m_addressV,
				option.m_addressW,
				option.m_mipLODBias,
				1,	/*anisotropy option set later*/
				option.m_comparisonFunc,
				{ 1.f, 1.f, 1.f, 1.f },
				std::numeric_limits<float>::min(),
				std::numeric_limits<float>::max()
			};

			SamplerState state( trait );
			m_samplerStates.emplace( option, state );

			return state;
		}

		return found->second;
	}

	VertexLayout AbstractGraphicsInterface::FindOrCreate( const VertexShader& vs, const VertexLayoutDesc& desc )
	{
		if ( desc.Size() == 0 )
		{
			return {};
		}

		auto found = m_vertexLayouts.find( desc );
		if ( found == m_vertexLayouts.end() )
		{
			VertexLayout vertexLayout( vs, desc );
			m_vertexLayouts.emplace( desc, vertexLayout );

			return vertexLayout;
		}

		return found->second;
	}

	BinaryChunk AbstractGraphicsInterface::CompieShader( const BinaryChunk& source, std::vector<const char*>& defines, const char* profile ) const
	{
		return m_agl->CompileShader( source, defines, profile );
	}

	bool AbstractGraphicsInterface::BuildShaderMetaData( const BinaryChunk& byteCode, agl::ShaderParameterMap& outParameterMap, agl::ShaderParameterInfo& outParameterInfo ) const
	{
		return m_agl->BuildShaderMetaData( byteCode, outParameterMap, outParameterInfo );
	}

	AbstractGraphicsInterface& GraphicsInterface()
	{
		return g_abstractGraphicsInterface;
	}
}
