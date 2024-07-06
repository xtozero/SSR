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

	void AbstractGraphicsInterface::UnLock( agl::Buffer* buffer, uint32 subResource )
	{
		m_agl->UnLock( buffer, subResource );
	}

	agl::LockedResource AbstractGraphicsInterface::Lock( agl::Texture* texture, agl::ResourceLockFlag lockFlag, uint32 subResource )
	{
		return m_agl->Lock( texture, lockFlag, subResource );
	}

	void AbstractGraphicsInterface::UnLock( agl::Texture* texture, uint32 subResource )
	{
		m_agl->UnLock( texture, subResource );
	}

	agl::ICommandList* AbstractGraphicsInterface::GetCommandList()
	{
		return m_agl->GetCommandList();
	}

	agl::ICommandList* AbstractGraphicsInterface::GetParallelCommandList()
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
				auto& dest = trait.m_renderTarget[i];
				auto& src = option.m_renderTarget[i];

				dest.m_blendEnable = src.m_blendEnable;
				dest.m_srcBlend = src.m_srcBlend;
				dest.m_destBlend = src.m_destBlend;
				dest.m_blendOp = src.m_blendOp;
				dest.m_srcBlendAlpha = src.m_srcBlendAlpha;
				dest.m_destBlendAlpha = src.m_destBlendAlpha;
				dest.m_blendOpAlpha = src.m_blendOpAlpha;
				dest.m_renderTargetWriteMask = src.m_renderTargetWriteMask;
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
				.m_depthEnable = option.m_depth.m_enable,
				.m_depthWriteMode = option.m_depth.m_writeDepth ? agl::DepthWriteMode::All : agl::DepthWriteMode::Zero,
				.m_depthFunc = option.m_depth.m_depthFunc,
				.m_stencilEnable = option.m_stencil.m_enable,
				.m_stencilReadMask = option.m_stencil.m_readMask,
				.m_stencilWriteMask = option.m_stencil.m_writeMask,
				.m_frontFace = option.m_stencil.m_frontFace,
				.m_backFace = option.m_stencil.m_backFace
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
			agl::RasterizerStateTrait trait = {
				.m_fillMode = option.m_isWireframe ? agl::FillMode::Wireframe : agl::FillMode::Solid,
				.m_cullMode = option.m_cullMode,
				.m_frontCounterClockwise = option.m_counterClockwise,
				.m_depthBias = option.m_depthBias,
				.m_depthBiasClamp = 0.f,
				.m_slopeScaleDepthBias = 0.f,
				.m_depthClipEnable = option.m_depthClipEnable,
				.m_scissorEnable = option.m_scissorEnable,
				.m_multisampleEnable = option.m_multisampleEnable,
				.m_antialiasedLineEnable = option.m_antialiasedLineEnable
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
			agl::SamplerStateTrait trait = {
				.m_filter = option.m_filter,
				.m_addressU = option.m_addressU,
				.m_addressV = option.m_addressV,
				.m_addressW = option.m_addressW,
				.m_mipLODBias = option.m_mipLODBias,
				.m_maxAnisotropy = 1,	/*anisotropy option set later*/
				.m_comparisonFunc = option.m_comparisonFunc,
				.m_borderColor = option.m_borderColor,
				.m_minLOD = std::numeric_limits<float>::min(),
				.m_maxLOD = std::numeric_limits<float>::max()
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

		VertexLayoutInstance vertexLayoutInstance = {
			.m_vertexShader = vs.Resource(),
			.m_desc = desc
		};

		auto found = m_vertexLayouts.find( vertexLayoutInstance );
		if ( found == m_vertexLayouts.end() )
		{
			VertexLayout vertexLayout( vs, desc );
			m_vertexLayouts.emplace( vertexLayoutInstance, vertexLayout );

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
