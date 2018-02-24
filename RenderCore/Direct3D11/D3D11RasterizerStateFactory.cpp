#include "stdafx.h"
#include "D3D11RasterizerStateFactory.h"

#include "D3D11RenderState.h"
#include "D3D11RenderStateManager.h"

#include "../../Engine/EnumStringMap.h"
#include "../../Engine/KeyValueReader.h"
#include "../../Shared/Util.h"

#include <D3D11.h>
#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/RasterizerStateDesc.txt" );
	constexpr TCHAR* RASTERIZER_DESC_HANDLER_KEY_NAME = _T( "RasterizerDesc" );
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
