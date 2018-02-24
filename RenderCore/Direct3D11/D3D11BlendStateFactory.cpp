#include "stdafx.h"
#include "D3D11BlendStateFactory.h"

#include "D3D11RenderStateManager.h"

#include "../../Engine/EnumStringMap.h"
#include "../../Engine/KeyValueReader.h"
#include "../../Shared/Util.h"

#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/BlendStateDesc.txt" );
	constexpr TCHAR* RASTERIZER_DESC_HANDLER_KEY_NAME = _T( "BlendDesc" );
}

void CD3D11BlendStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( RASTERIZER_STATE_DESC_FILE_NAME );

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
