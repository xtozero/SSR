#include "stdafx.h"
#include "D3D11SamplerStateFactory.h"

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
	constexpr TCHAR* SAMPLER_STATE_DESC_FILE_NAME = _T( "../Script/SamplerStateDesc.txt" );
	constexpr TCHAR* SMAPLER_DESC_HANDLER_KEY_NAME = _T( "SamplerDesc" );
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
