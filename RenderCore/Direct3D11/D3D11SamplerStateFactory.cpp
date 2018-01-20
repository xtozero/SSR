#include "stdafx.h"
#include "D3D11SamplerStateFactory.h"

#include "D3D11RenderStateManager.h"

#include "../CommonRenderer/IRenderState.h"

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

class CSamplerState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device& device, const D3D11_SAMPLER_DESC& samplerDesc );

	CSamplerState( CD3D11RenderStateManager& renderStateManager ) : m_renderStateManager( renderStateManager ) { }
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
	CD3D11RenderStateManager& m_renderStateManager;
};

void CSamplerState::Set( const SHADER_TYPE type )
{
	assert( type != SHADER_TYPE::NONE );

	switch ( type )
	{
	case SHADER_TYPE::VS:
		m_renderStateManager.SetVsSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::HS:
		m_renderStateManager.SetHsSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::DS:
		m_renderStateManager.SetDsSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::GS:
		m_renderStateManager.SetGsSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::PS:
		m_renderStateManager.SetPsSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::CS:
		m_renderStateManager.SetCsSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	default:
		DebugWarning( "InValid SamplerState SHADER_TYPE" );
		break;
	}
}

bool CSamplerState::Create( ID3D11Device& device, const D3D11_SAMPLER_DESC & samplerDesc )
{
	HRESULT hr = device.CreateSamplerState( &samplerDesc, &m_pSamplerState );
	return SUCCEEDED( hr );
}

class CNullSamplerState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = NONE ) override;
};

void CNullSamplerState::Set( const SHADER_TYPE  )
{

}

void CD3D11SamplerStateFactory::OnDeviceLost( )
{
	m_samplerState.clear( );
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

IRenderState* CD3D11SamplerStateFactory::GetSamplerState( ID3D11Device& device, CD3D11RenderStateManager& renderStateManager, const String& stateName )
{
	auto foundState = m_samplerState.find( stateName );

	if ( foundState != m_samplerState.end( ) )
	{
		return foundState->second.get( );
	}

	auto foundDesc = m_samplerStateDesc.find( stateName );

	if ( foundDesc != m_samplerStateDesc.end( ) )
	{
		std::unique_ptr<CSamplerState> newState = std::make_unique<CSamplerState>( renderStateManager );

		if ( newState->Create( device, foundDesc->second ) )
		{
			CSamplerState* ret = newState.get( );
			m_samplerState.emplace( stateName, std::move( newState ) );
			return ret;
		}
	}

	std::unique_ptr<CNullSamplerState> nullState = std::make_unique<CNullSamplerState>();
	CNullSamplerState* ret = nullState.get( );
	m_samplerState.emplace( _T( "NULL" ), std::move( nullState ) );
	return ret;
}

void CD3D11SamplerStateFactory::AddSamplerDesc( const String& descName, const D3D11_SAMPLER_DESC& newDesc )
{
	m_samplerStateDesc.emplace( descName, newDesc );
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

		AddSamplerDesc( desc->GetValue( ), newDesc );
	}
}
