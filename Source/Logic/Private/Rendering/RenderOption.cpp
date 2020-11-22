#include "stdafx.h"
#include "Rendering/RenderOption.h"

#include "FileSystem/EngineFileSystem.h"
#include "Json/json.hpp"
#include "Rendering/ShaderAsset.h"

void BlendOption::LoadFromAsset( const JSON::Value& blendOption, const AssetLoaderSharedHandle& /*handle*/ )
{
	if ( const JSON::Value* alphaToConverage = blendOption.Find( "AlphaToConverage" ) )
	{
		m_alphaToConverageEnable = alphaToConverage->AsBool( );
	}

	if ( const JSON::Value* independentBlendEnable = blendOption.Find( "IndependentBlendEnable" ) )
	{
		m_independentBlendEnable = independentBlendEnable->AsBool( );
	}

	if ( const JSON::Value* pRenderTargets = blendOption.Find( "RenderTargets" ) )
	{
		const JSON::Value& renderTargets = *pRenderTargets;

		for ( int i = 0; i < renderTargets.Size( ); ++i )
		{
			if ( const JSON::Value* blendEnable = renderTargets[i].Find( "BlendEnable" ) )
			{
				m_renderTarget[i].m_blendEnable = blendEnable->AsBool( );
			}

			if ( const JSON::Value* srcBlend = renderTargets[i].Find( "SrcBlend" ) )
			{
				m_renderTarget[i].m_srcBlend = GetEnum( srcBlend->AsString( ), BLEND::ONE );
			}

			if ( const JSON::Value* destBlend = renderTargets[i].Find( "DestBlend" ) )
			{
				m_renderTarget[i].m_destBlend = GetEnum( destBlend->AsString( ), BLEND::ZERO );
			}

			if ( const JSON::Value* blendOp = renderTargets[i].Find( "BlendOp" ) )
			{
				m_renderTarget[i].m_blendOp = GetEnum( blendOp->AsString( ), BLEND_OP::ADD );
			}

			if ( const JSON::Value* srcBlendAlpha = renderTargets[i].Find( "SrcBlendAlpha" ) )
			{
				m_renderTarget[i].m_srcBlendAlpha = GetEnum( srcBlendAlpha->AsString( ), BLEND::ONE );
			}

			if ( const JSON::Value* destBlendAlpha = renderTargets[i].Find( "DestBlendAlpha" ) )
			{
				m_renderTarget[i].m_destBlendAlpha = GetEnum( destBlendAlpha->AsString( ), BLEND::ZERO );
			}

			if ( const JSON::Value* blendOpAlpha = renderTargets[i].Find( "BlendOpAlpha" ) )
			{
				m_renderTarget[i].m_blendOpAlpha = GetEnum( blendOpAlpha->AsString( ), BLEND_OP::ADD );
			}

			if ( const JSON::Value* renderTargetWriteMask = renderTargets[i].Find( "RenderTargetWriteMask" ) )
			{
				for ( const JSON::Value& mask : *renderTargetWriteMask )
				{
					m_renderTarget[i].m_renderTargetWriteMask = GetEnum( mask.AsString( ), COLOR_WRITE_ENABLE::ALL );
				}
			}
		}
	}
}

void DepthStencilOption::LoadFromAsset( const JSON::Value& depthStencilOption, const AssetLoaderSharedHandle& /*handle*/ )
{
	if ( const JSON::Value* depthEnable = depthStencilOption.Find( "DepthEnable" ) )
	{
		m_depth.m_enable = depthEnable->AsBool( );
	}

	if ( const JSON::Value* depthWriteEnable = depthStencilOption.Find( "DepthWriteEnable" ) )
	{
		m_depth.m_writeDepth = depthWriteEnable->AsBool( );
	}

	if ( const JSON::Value* depthFunc = depthStencilOption.Find( "DepthFunc" ) )
	{
		m_depth.m_depthFunc = GetEnum( depthFunc->AsString( ), COMPARISON_FUNC::LESS );
	}

	if ( const JSON::Value* stencilEnable = depthStencilOption.Find( "StencilEnable" ) )
	{
		m_stencil.m_enable = stencilEnable->AsBool( );
	}

	if ( const JSON::Value* stencilReadMask = depthStencilOption.Find( "StencilReadMask" ) )
	{
		m_stencil.m_readMask = static_cast<unsigned char>( stencilReadMask->AsInt( ) );
	}

	if ( const JSON::Value* stencilWriteMask = depthStencilOption.Find( "StencilWriteMask" ) )
	{
		m_stencil.m_writeMask = static_cast<unsigned char>( stencilWriteMask->AsInt( ) );
	}

	if ( const JSON::Value* stencilRef = depthStencilOption.Find( "StencilRef" ) )
	{
		m_stencil.m_ref = static_cast<unsigned int>( stencilRef->AsInt( ) );
	}

	STENCIL_OP_TRAIT& front = m_stencil.m_frontFace;
	if ( const JSON::Value* frontFaceStencilFunc = depthStencilOption.Find( "FrontFace.StencilFunc" ) )
	{
		front.m_func = GetEnum( frontFaceStencilFunc->AsString( ), COMPARISON_FUNC::ALWAYS );
	}

	if ( const JSON::Value* frontFaceStencilDepthFailOp = depthStencilOption.Find( "FrontFace.StencilDepthFailOp" ) )
	{
		front.m_depthFailOp = GetEnum( frontFaceStencilDepthFailOp->AsString( ), STENCIL_OP::KEEP );
	}

	if ( const JSON::Value* frontFaceStencilPassOp = depthStencilOption.Find( "FrontFace.StencilPassOp" ) )
	{
		front.m_passOp = GetEnum( frontFaceStencilPassOp->AsString( ), STENCIL_OP::KEEP );
	}

	if ( const JSON::Value* frontFaceStencilFailOp = depthStencilOption.Find( "FrontFace.StencilFailOp" ) )
	{
		front.m_failOp = GetEnum( frontFaceStencilFailOp->AsString( ), STENCIL_OP::KEEP );
	}

	STENCIL_OP_TRAIT& back = m_stencil.m_frontFace;
	if ( const JSON::Value* backFaceStencilFunc = depthStencilOption.Find( "BackFace.StencilFunc" ) )
	{
		back.m_func = GetEnum( backFaceStencilFunc->AsString( ), COMPARISON_FUNC::ALWAYS );
	}

	if ( const JSON::Value* backFaceStencilDepthFailOp = depthStencilOption.Find( "BackFace.StencilDepthFailOp" ) )
	{
		back.m_depthFailOp = GetEnum( backFaceStencilDepthFailOp->AsString( ), STENCIL_OP::KEEP );
	}

	if ( const JSON::Value* backFaceStencilPassOp = depthStencilOption.Find( "BackFace.StencilPassOp" ) )
	{
		back.m_passOp = GetEnum( backFaceStencilPassOp->AsString( ), STENCIL_OP::KEEP );
	}

	if ( const JSON::Value* backFaceStencilFailOp = depthStencilOption.Find( "BackFace.StencilFailOp" ) )
	{
		back.m_failOp = GetEnum( backFaceStencilFailOp->AsString( ), STENCIL_OP::KEEP );
	}
}

void RasterizerOption::LoadFromAsset( const JSON::Value& rasterizerOption, const AssetLoaderSharedHandle& /*handle*/ )
{
	if ( const JSON::Value* wireframe = rasterizerOption.Find( "Wireframe" ) )
	{
		m_isWireframe = wireframe->AsBool( );
	}

	if ( const JSON::Value* cullMode = rasterizerOption.Find( "CullMode" ) )
	{
		m_cullMode = GetEnum( cullMode->AsString( ), CULL_MODE::BACK );
	}

	if ( const JSON::Value* counterClockwise = rasterizerOption.Find( "CounterClockwise" ) )
	{
		m_counterClockwise = counterClockwise->AsBool( );
	}

	if ( const JSON::Value* depthBias = rasterizerOption.Find( "DepthBias" ) )
	{
		m_depthBias = depthBias->AsInt( );
	}

	if ( const JSON::Value* depthClipEnable = rasterizerOption.Find( "DepthClipEnable" ) )
	{
		m_depthClipEnable = depthClipEnable->AsBool( );
	}

	if ( const JSON::Value* scissorEnable = rasterizerOption.Find( "ScissorEnable" ) )
	{
		m_scissorEnable = scissorEnable->AsBool( );
	}

	if ( const JSON::Value* multisampleEnalbe = rasterizerOption.Find( "MultisampleEnable" ) )
	{
		m_multisampleEnalbe = multisampleEnalbe->AsBool( );
	}

	if ( const JSON::Value* antialiasedLineEnable = rasterizerOption.Find( "AntialiasedLineEnable" ) )
	{
		m_antialiasedLineEnable = antialiasedLineEnable->AsInt( );
	}
}

void SamplerOption::LoadFromAsset( const JSON::Value& samplerOption, const AssetLoaderSharedHandle& /*handle*/ )
{
	if ( const JSON::Value* filter = samplerOption.Find( "Filter" ) )
	{
		m_filter = 0;
		for ( auto option : *filter )
		{
			m_filter |= GetEnum( option.AsString( ), 0 );
		}
	}

	if ( const JSON::Value* addressU = samplerOption.Find( "AddressU" ) )
	{
		m_addressU = GetEnum( addressU->AsString( ), TEXTURE_ADDRESS_MODE::CLAMP );
	}

	if ( const JSON::Value* addressV = samplerOption.Find( "AddressV" ) )
	{
		m_addressV = GetEnum( addressV->AsString( ), TEXTURE_ADDRESS_MODE::CLAMP );
	}

	if ( const JSON::Value* addressW = samplerOption.Find( "AddressW" ) )
	{
		m_addressW = GetEnum( addressW->AsString( ), TEXTURE_ADDRESS_MODE::CLAMP );
	}

	if ( const JSON::Value* mipLODBias = samplerOption.Find( "MipLODBias" ) )
	{
		m_mipLODBias = static_cast<float>( mipLODBias->AsReal( ) );
	}

	if ( const JSON::Value* comparisonFunc = samplerOption.Find( "ComparisonFunc" ) )
	{
		m_comparisonFunc = GetEnum( comparisonFunc->AsString( ), COMPARISON_FUNC::NEVER );
	}
}

void RenderOption::LoadFromAsset( const JSON::Value& renderOption, const AssetLoaderSharedHandle& handle )
{
	if ( const JSON::Value* shaderKeys = renderOption.Find( "Shader" ) )
	{
		for ( int shaderType = static_cast<int>( SHADER_TYPE::VS ); shaderType < static_cast<int>( SHADER_TYPE::Count ); ++shaderType )
		{
			const JSON::Value* shaderFilePath = shaderKeys->Find( ToString( static_cast<SHADER_TYPE>( shaderType ) ) );
			if ( shaderFilePath == nullptr )
			{
				continue;
			}

			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindFunctor(
				[this, shaderType]( void* shader )
				{
					switch( static_cast<SHADER_TYPE>( shaderType ) )
					{
					case SHADER_TYPE::VS:
						m_vertexShader = static_cast<VertexShaderAsset*>( shader )->m_shader;
						break;
					case SHADER_TYPE::PS:
						m_pixelShader = static_cast<PixelShaderAsset*>( shader )->m_shader;
						break;
					}
				}
			);

			auto hRequest = GetInterface<IAssetLoader>( )->RequestAsyncLoad( shaderFilePath->AsString( ), onLoadComplete );
			if ( hRequest->IsLoadingInProgress( ) )
			{
				handle->AddPrerequisite( hRequest );
			}
		}
	}

	if ( const JSON::Value* blendOptionKey = renderOption.Find( "Blend" ) )
	{
		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunctor(
			[this]( void* blendOption )
			{
				m_blendOption = static_cast<BlendOption*>( blendOption );
			}
		);

		auto hRequest = GetInterface<IAssetLoader>( )->RequestAsyncLoad( blendOptionKey->AsString( ), onLoadComplete );
		if ( hRequest->IsLoadingInProgress( ) )
		{
			handle->AddPrerequisite( hRequest );
		}
	}

	if ( const JSON::Value* depthStencilOptionKey = renderOption.Find( "DS_State" ) )
	{
		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunctor(
			[this]( void* depthStencilOption )
			{
				m_depthStencilOption = static_cast<DepthStencilOption*>( depthStencilOption );
			}
		);

		auto hRequest = GetInterface<IAssetLoader>( )->RequestAsyncLoad( depthStencilOptionKey->AsString( ), onLoadComplete );
		if ( hRequest->IsLoadingInProgress( ) )
		{
			handle->AddPrerequisite( hRequest );
		}
	}

	if ( const JSON::Value* rasterizerOptionKey = renderOption.Find( "RS_State" ) )
	{
		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunctor(
			[this]( void* rasterizerOption )
			{
				m_rasterizerOption = static_cast<RasterizerOption*>( rasterizerOption );
			}
		);

		auto hRequest = GetInterface<IAssetLoader>( )->RequestAsyncLoad( rasterizerOptionKey->AsString( ), onLoadComplete );
		if ( hRequest->IsLoadingInProgress( ) )
		{
			handle->AddPrerequisite( hRequest );
		}
	}

	if ( const JSON::Value* samplerOptionKeys = renderOption.Find( "Sampler" ) )
	{
		for ( int shaderType = static_cast<int>( SHADER_TYPE::VS ); shaderType < static_cast<int>( SHADER_TYPE::Count ); ++shaderType )
		{
			const JSON::Value* samplerOptionKey = samplerOptionKeys->Find( ToString( static_cast<SHADER_TYPE>( shaderType ) ) );
			if ( samplerOptionKey == nullptr )
			{
				continue;
			}

			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindFunctor(
				[this, shaderType]( void* samplerOption )
				{
					m_samplerOption[shaderType][0] = static_cast<SamplerOption*>( samplerOption );
				}
			);

			auto hRequest = GetInterface<IAssetLoader>( )->RequestAsyncLoad( samplerOptionKey->AsString( ), onLoadComplete );
			if ( hRequest->IsLoadingInProgress( ) )
			{
				handle->AddPrerequisite( hRequest );
			}
		}
	}
}
