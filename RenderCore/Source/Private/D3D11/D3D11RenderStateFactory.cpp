#include "stdafx.h"
#include "D3D11/D3D11RenderStateFactory.h"

#include "D3D11/D3D11RenderState.h"
#include "D3D11/D3D11RenderStateManager.h"
#include "DataStructure/EnumStringMap.h"
#include "DataStructure/KeyValueReader.h"
#include "Util.h"

#include <D3D11.h>
#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* SAMPLER_STATE_DESC_FILE_NAME = _T( "../Script/SamplerStateDesc.txt" );
	constexpr TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/RasterizerStateDesc.txt" );
	constexpr TCHAR* BLEND_STATE_DESC_FILE_NAME = _T( "../Script/BlendStateDesc.txt" );
	constexpr TCHAR* DEPTH_STENCIL_DESC_FILE_NAME = _T( "../Script/DepthStencilDesc.txt" );
}

void CD3D11SamplerStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( SAMPLER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadSamplerDesc( *pKeyValues );
	}
}

Owner<CD3D11SamplerState*> CD3D11SamplerStateFactory::GetSamplerState( ID3D11Device& device, const String& stateName )
{
	auto foundDesc = m_samplerStateDesc.find( stateName );

	if ( foundDesc != m_samplerStateDesc.end( ) )
	{
		CD3D11SamplerState* newState = new CD3D11SamplerState;

		if ( newState->Create( device, foundDesc->second ) )
		{
			newState->SetName( stateName );
			return newState;
		}
	}

	return nullptr;
}

void CD3D11SamplerStateFactory::LoadSamplerDesc( const KeyValue& keyValue )
{
	for ( auto desc = keyValue.GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		CD3D11_DEFAULT default;
		CD3D11_SAMPLER_DESC newDesc( default );

		if ( const KeyValue* pFilter = desc->Find( _T( "Filter" ) ) )
		{
			newDesc.Filter = static_cast<D3D11_FILTER>( GetEnumStringMap( ).GetEnum( pFilter->GetValue( ), D3D11_FILTER_MIN_MAG_MIP_LINEAR ) );
		}

		if ( const KeyValue* pAddressU = desc->Find( _T( "AddressU" ) ) )
		{
			newDesc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>( GetEnumStringMap( ).GetEnum( pAddressU->GetValue( ), D3D11_TEXTURE_ADDRESS_CLAMP ) );
		}

		if ( const KeyValue* pAddressV = desc->Find( _T( "AddressV" ) ) )
		{
			newDesc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>( GetEnumStringMap( ).GetEnum( pAddressV->GetValue( ), D3D11_TEXTURE_ADDRESS_CLAMP ) );
		}

		if ( const KeyValue* pAddressW = desc->Find( _T( "AddressW" ) ) )
		{
			newDesc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>( GetEnumStringMap( ).GetEnum( pAddressW->GetValue( ), D3D11_TEXTURE_ADDRESS_CLAMP ) );
		}

		if ( const KeyValue* pMipLODBias = desc->Find( _T( "MipLODBias" ) ) )
		{
			newDesc.MipLODBias = pMipLODBias->GetValue<float>( );
		}

		if ( const KeyValue* pMaxAnisotropy = desc->Find( _T( "MaxAnisotropy" ) ) )
		{
			newDesc.MaxAnisotropy = pMaxAnisotropy->GetValue<UINT>( );
		}

		if ( const KeyValue* pComparisonFunc = desc->Find( _T( "ComparisonFunc" ) ) )
		{
			newDesc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>( GetEnumStringMap( ).GetEnum( pComparisonFunc->GetValue( ), D3D11_COMPARISON_LESS ) );
		}

		if ( const KeyValue* pBorderColor = desc->Find( _T( "BorderColor" ) ) )
		{
			Stringstream sStream;
			sStream << pBorderColor->GetValue( );

			sStream >> newDesc.BorderColor[0] >> newDesc.BorderColor[1] >> newDesc.BorderColor[2] >> newDesc.BorderColor[3];
		}

		if ( const KeyValue* pMinLOD = desc->Find( _T( "MinLOD" ) ) )
		{
			newDesc.MinLOD = pMinLOD->GetValue<float>( );
		}

		if ( const KeyValue* pMaxLOD = desc->Find( _T( "MaxLOD" ) ) )
		{
			newDesc.MaxLOD = pMaxLOD->GetValue<float>( );
		}

		m_samplerStateDesc.emplace( desc->GetValue( ), newDesc );
	}
}

void CD3D11RasterizerStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( RASTERIZER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadRasterizerDesc( *pKeyValues );
	}
}

Owner<CD3D11RasterizerState*> CD3D11RasterizerStateFactory::GetRasterizerState( ID3D11Device& device, const String& stateName )
{
	auto foundDesc = m_rasterizerStateDesc.find( stateName );

	if ( foundDesc != m_rasterizerStateDesc.end( ) )
	{
		CD3D11RasterizerState* newState = new CD3D11RasterizerState;

		if ( newState->Create( device, foundDesc->second ) )
		{
			newState->SetName( stateName );
			return newState;
		}
		
		delete newState;
	}

	return nullptr;
}

void CD3D11RasterizerStateFactory::LoadRasterizerDesc( const KeyValue& keyValue )
{
	for ( auto desc = keyValue.GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		CD3D11_DEFAULT default;
		CD3D11_RASTERIZER_DESC newDesc( default );

		if ( const KeyValue* pFillMode = desc->Find( _T( "FillMode" ) ) )
		{
			newDesc.FillMode = static_cast<D3D11_FILL_MODE>( GetEnumStringMap( ).GetEnum( pFillMode->GetValue( ), D3D11_FILL_SOLID ) );
		}
		
		if ( const KeyValue* pCullMode = desc->Find( _T( "CullMode" ) ) )
		{
			newDesc.CullMode = static_cast<D3D11_CULL_MODE>( GetEnumStringMap( ).GetEnum( pCullMode->GetValue( ), D3D11_CULL_BACK ) );
		}
		
		if ( const KeyValue* pFrontCounterClockwise = desc->Find( _T( "FrontCounterClockwise" ) ) )
		{
			newDesc.FrontCounterClockwise = pFrontCounterClockwise->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pDepthBias = desc->Find( _T( "DepthBias" ) ) )
		{
			newDesc.DepthBias = pDepthBias->GetValue<int>( );
		}
		
		if ( const KeyValue* pDepthBiasClamp = desc->Find( _T( "DepthBiasClamp" ) ) )
		{
			newDesc.DepthBiasClamp = pDepthBiasClamp->GetValue<float>( );
		}
		
		if ( const KeyValue* pSlopeScaledDepthBias = desc->Find( _T( "SlopeScaledDepthBias" ) ) )
		{
			newDesc.SlopeScaledDepthBias = pSlopeScaledDepthBias->GetValue<float>( );
		}
		
		if ( const KeyValue* pDepthClipEnable = desc->Find( _T( "DepthClipEnable" ) ) )
		{
			newDesc.DepthClipEnable = pDepthClipEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pScissorEnable = desc->Find( _T( "ScissorEnable" ) ) )
		{
			newDesc.ScissorEnable = pScissorEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pMultisampleEnable = desc->Find( _T( "MultisampleEnable" ) ) )
		{
			newDesc.MultisampleEnable = pMultisampleEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pAntialiasedLineEnable = desc->Find( _T( "AntialiasedLineEnable" ) ) )
		{
			newDesc.AntialiasedLineEnable = pAntialiasedLineEnable->GetValue<int>( ) != 0;
		}

		m_rasterizerStateDesc.emplace( desc->GetValue( ), newDesc );
	}
}

void CD3D11BlendStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( BLEND_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadRasterizerDesc( *pKeyValues );
	}
}

Owner<CD3D11BlendState*> CD3D11BlendStateFactory::GetBlendState( ID3D11Device& device, const String& stateName )
{
	auto foundDesc = m_blendStateDesc.find( stateName );

	if ( foundDesc != m_blendStateDesc.end( ) )
	{
		CD3D11BlendState* newState = new CD3D11BlendState;

		if ( newState->Create( device, foundDesc->second ) )
		{
			newState->SetName( stateName );
			return newState;
		}

		delete newState;
	}

	return nullptr;
}

void CD3D11BlendStateFactory::LoadRasterizerDesc( const KeyValue& keyValue )
{
	for ( auto desc = keyValue.GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		CD3D_BLEND_DESC newDesc;
		CD3D11_BLEND_DESC& blendDesc = newDesc.m_desc;

		if ( const KeyValue* pAlphaToCoverageEnable = desc->Find( _T( "AlphaToCoverageEnable" ) ) )
		{
			blendDesc.AlphaToCoverageEnable = pAlphaToCoverageEnable->GetValue<int>( ) != 0;
		}
		else if ( const KeyValue* pIndependentBlendEnable = desc->Find( _T( "IndependentBlendEnable" ) ) )
		{
			blendDesc.IndependentBlendEnable = pIndependentBlendEnable->GetValue<int>( ) != 0;
		}
		else if ( const KeyValue* pRenderTarget = desc->Find( _T( "RenderTarget" ) ) )
		{
			int idx = pRenderTarget->GetValue<int>( );

			assert( idx >= 0 && idx < 8 );

			D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[idx];

			if ( const KeyValue* pBlendEnable = pRenderTarget->Find( _T( "BlendEnable" ) ) )
			{
				rtBlendDesc.BlendEnable = pBlendEnable->GetValue<int>( ) != 0;
			}

			if ( const KeyValue* pBlendOp = pRenderTarget->Find( _T( "BlendOp" ) ) )
			{
				rtBlendDesc.BlendOp = static_cast<D3D11_BLEND_OP>( GetEnumStringMap( ).GetEnum( pBlendOp->GetValue( ), D3D11_BLEND_OP_ADD ) );
			}

			if ( const KeyValue* pBlendOpAlpha = pRenderTarget->Find( _T( "BlendOpAlpha" ) ) )
			{
				rtBlendDesc.BlendOpAlpha = static_cast<D3D11_BLEND_OP>( GetEnumStringMap( ).GetEnum( pBlendOpAlpha->GetValue( ), D3D11_BLEND_OP_ADD ) );
			}

			if ( const KeyValue* pDestBlend = pRenderTarget->Find( _T( "DestBlend" ) ) )
			{
				rtBlendDesc.DestBlend = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pDestBlend->GetValue( ), D3D11_BLEND_ZERO ) );
			}

			if ( const KeyValue* pDestBlendAlpha = pRenderTarget->Find( _T( "DestBlendAlpha" ) ) )
			{
				rtBlendDesc.DestBlendAlpha = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pDestBlendAlpha->GetValue( ), D3D11_BLEND_ZERO ) );
			}

			if ( const KeyValue* pSrcBlend = pRenderTarget->Find( _T( "SrcBlend" ) ) )
			{
				rtBlendDesc.SrcBlend = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pSrcBlend->GetValue( ), D3D11_BLEND_ONE ) );
			}

			if ( const KeyValue* pSrcBlendAlpha = pRenderTarget->Find( _T( "SrcBlendAlpha" ) ) )
			{
				rtBlendDesc.SrcBlendAlpha = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pSrcBlendAlpha->GetValue( ), D3D11_BLEND_ONE ) );
			}

			if ( const KeyValue* pRenderTargetWriteMask = pRenderTarget->Find( _T( "RenderTargetWriteMask" ) ) )
			{
				rtBlendDesc.RenderTargetWriteMask = static_cast<UINT8>( GetEnumStringMap( ).GetEnum( pRenderTargetWriteMask->GetValue( ), D3D11_COLOR_WRITE_ENABLE_ALL ) );
			};
		}
		else if ( const KeyValue* pBlendFactor = desc->Find( _T( "BlendFactor" ) ) )
		{
			Stringstream ss( pBlendFactor->GetValue( ) );

			for ( auto factor = newDesc.m_blendFactor.begin( ); ss && factor != newDesc.m_blendFactor.end( ); ++factor )
			{
				ss >> *factor;
			}
		}
		else if ( const KeyValue* pSampleMask = desc->Find( _T( "SampleMask" ) ) )
		{
			Stringstream ss( pSampleMask->GetValue( ) );
			ss >> newDesc.m_sampleMask;
		}

		m_blendStateDesc.emplace( desc->GetValue( ), newDesc );
	}
}

void CD3D11DepthStencilStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( DEPTH_STENCIL_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadDepthStencilDesc( *pKeyValues );
	}
}

Owner<CD3D11DepthStencilState*> CD3D11DepthStencilStateFactory::GetDepthStencilState( ID3D11Device& device, const String& stateName )
{
	auto foundDesc = m_depthStencilDesc.find( stateName );

	if ( foundDesc != m_depthStencilDesc.end( ) )
	{
		CD3D11DepthStencilState* newState = new CD3D11DepthStencilState;

		if ( newState->Create( device, foundDesc->second ) )
		{
			newState->SetName( stateName );
			return newState;
		}

		delete newState;
	}

	return nullptr;
}

void CD3D11DepthStencilStateFactory::AddDepthStencilDesc( const String& descName, const CD3D11DepthStencilDesc& newDesc )
{
	m_depthStencilDesc.emplace( descName, newDesc );
}

void CD3D11DepthStencilStateFactory::LoadDepthStencilDesc( const KeyValue& keyValue )
{
	for ( auto desc = keyValue.GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		CD3D11_DEFAULT default;
		CD3D11DepthStencilDesc newDesc( default );

		if ( const KeyValue* pDepthEnable = desc->Find( _T( "DepthEnable" ) ) )
		{
			newDesc.DepthEnable = pDepthEnable->GetValue<int>( ) != 0;
		}

		if ( const KeyValue* pDepthWriteMask = desc->Find( _T( "DepthWriteMask" ) ) )
		{
			newDesc.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>( GetEnumStringMap( ).GetEnum( pDepthWriteMask->GetValue( ), D3D11_DEPTH_WRITE_MASK_ALL ) );
		}

		if ( const KeyValue* pDepthFunc = desc->Find( _T( "DepthFunc" ) ) )
		{
			newDesc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>( GetEnumStringMap( ).GetEnum( pDepthFunc->GetValue( ), D3D11_COMPARISON_LESS ) );
		}

		if ( const KeyValue* pStencilEnable = desc->Find( _T( "StencilEnable" ) ) )
		{
			newDesc.StencilEnable = pStencilEnable->GetValue<int>( ) != 0;
		}

		if ( const KeyValue* pStencilReadMask = desc->Find( _T( "StencilReadMask" ) ) )
		{
			newDesc.StencilReadMask = static_cast<UINT8>( pStencilReadMask->GetValue<unsigned int>( ) );
		}

		if ( const KeyValue* pStencilWriteMask = desc->Find( _T( "StencilWriteMask" ) ) )
		{
			newDesc.StencilWriteMask = static_cast<UINT8>( pStencilWriteMask->GetValue<unsigned int>( ) );
		}

		if ( const KeyValue* pStencilRef = desc->Find( _T( "StencilRef" ) ) )
		{
			newDesc.StencilRef = static_cast<UINT>( pStencilRef->GetValue<UINT>( ) );
		}

		if ( const KeyValue* pStencilFunc = desc->Find( _T( "FrontFace.StencilFunc" ) ) )
		{
			newDesc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>( GetEnumStringMap( ).GetEnum( pStencilFunc->GetValue( ), D3D11_COMPARISON_ALWAYS ) );
		}

		if ( const KeyValue* pStencilFunc = desc->Find( _T( "BackFace.StencilFunc" ) ) )
		{
			newDesc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>( GetEnumStringMap( ).GetEnum( pStencilFunc->GetValue( ), D3D11_COMPARISON_ALWAYS ) );
		}

		if ( const KeyValue* pStencilDepthFailOp = desc->Find( _T( "FrontFace.StencilDepthFailOp" ) ) )
		{
			newDesc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilDepthFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}

		if ( const KeyValue* pStencilDepthFailOp = desc->Find( _T( "BackFace.StencilDepthFailOp" ) ) )
		{
			newDesc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilDepthFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}

		if ( const KeyValue* pStencilPassOp = desc->Find( _T( "FrontFace.StencilPassOp" ) ) )
		{
			newDesc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilPassOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}

		if ( const KeyValue* pStencilPassOp = desc->Find( _T( "BackFace.StencilPassOp" ) ) )
		{
			newDesc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilPassOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}

		if ( const KeyValue* pStencilFailOp = desc->Find( _T( "FrontFace.StencilFailOp" ) ) )
		{
			newDesc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}

		if ( const KeyValue* pStencilFailOp = desc->Find( _T( "BackFace.StencilFailOp" ) ) )
		{
			newDesc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}

		AddDepthStencilDesc( desc->GetValue( ), newDesc );
	}
}
