#include "stdafx.h"
#include "D3D11SamplerStateFactory.h"

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

	void SamplerDescHandler( CD3D11SamplerStateFactory* owner, const String&, const KeyValue* keyValue )
	{
		CD3D11_DEFAULT default;
		CD3D11_SAMPLER_DESC newDesc( default );

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "Filter" ) )
			{
				newDesc.Filter = static_cast<D3D11_FILTER>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_FILTER_MIN_MAG_MIP_LINEAR ));
			}
			else if ( property->GetKey( ) == _T( "AddressU" ) )
			{
				newDesc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_TEXTURE_ADDRESS_CLAMP ));
			}
			else if ( property->GetKey( ) == _T( "AddressV" ) )
			{
				newDesc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_TEXTURE_ADDRESS_CLAMP ));
			}
			else if ( property->GetKey( ) == _T( "AddressW" ) )
			{
				newDesc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_TEXTURE_ADDRESS_CLAMP ));
			}
			else if ( property->GetKey( ) == _T( "MipLODBias" ) )
			{
				newDesc.MipLODBias = property->GetValue<float>( );
			}
			else if ( property->GetKey( ) == _T( "MaxAnisotropy" ) )
			{
				newDesc.MaxAnisotropy = property->GetValue<UINT>( );
			}
			else if ( property->GetKey( ) == _T( "ComparisonFunc" ) )
			{
				newDesc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_COMPARISON_LESS ));
			}
			else if ( property->GetKey( ) == _T( "BorderColor" ) )
			{
				Stringstream sStream;
				sStream << property->GetValue( );

				sStream >> newDesc.BorderColor[0] >> newDesc.BorderColor[1] >> newDesc.BorderColor[2] >> newDesc.BorderColor[3];
			}
			else if ( property->GetKey( ) == _T( "MinLOD" ) )
			{
				newDesc.MinLOD = property->GetValue<float>( );
			}
			else if ( property->GetKey( ) == _T( "MaxLOD" ) )
			{
				newDesc.MaxLOD = property->GetValue<float>( );
			}
		}

		owner->AddSamplerDesc( keyValue->GetValue(), newDesc );
	}
}

class CSamplerState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device* pDevice, const D3D11_SAMPLER_DESC& samplerDesc );

	CSamplerState( ID3D11DeviceContext* pDeviceContext ) : m_pDeviceContext( pDeviceContext ) { }
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
};

void CSamplerState::Set( const SHADER_TYPE type )
{
	assert( type != SHADER_TYPE::NONE );

	switch ( type )
	{
	case SHADER_TYPE::VS:
		m_pDeviceContext->VSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::HS:
		m_pDeviceContext->HSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::DS:
		m_pDeviceContext->DSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::GS:
		m_pDeviceContext->GSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::PS:
		m_pDeviceContext->PSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::CS:
		m_pDeviceContext->CSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	default:
		DebugWarning( "InValid SamplerState SHADER_TYPE" );
		break;
	}
}

bool CSamplerState::Create( ID3D11Device * pDevice, const D3D11_SAMPLER_DESC & samplerDesc )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	HRESULT hr = pDevice->CreateSamplerState( &samplerDesc, &m_pSamplerState );
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

void CD3D11SamplerStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValueGroupImpl> pKeyValues = KeyValueReader.LoadKeyValueFromFile( SAMPLER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadSamplerDesc( pKeyValues.get() );
	}
}

IRenderState* CD3D11SamplerStateFactory::GetSamplerState( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& stateName )
{
	auto foundState = m_samplerState.find( stateName );

	if ( foundState != m_samplerState.end( ) )
	{
		return foundState->second.get( );
	}

	if ( pDevice )
	{
		auto foundDesc = m_samplerStateDesc.find( stateName );

		if ( foundDesc != m_samplerStateDesc.end( ) )
		{
			std::unique_ptr<CSamplerState> newState = std::make_unique<CSamplerState>( pDeviceContext );

			if ( newState->Create( pDevice, foundDesc->second ) )
			{
				CSamplerState* ret = newState.get( );
				m_samplerState.emplace( stateName, std::move( newState ) );
				return ret;
			}
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

CD3D11SamplerStateFactory::CD3D11SamplerStateFactory( )
{
	RegisterHandler( SMAPLER_DESC_HANDLER_KEY_NAME, SamplerDescHandler );
}

void CD3D11SamplerStateFactory::LoadSamplerDesc( KeyValueGroup* pKeyValues )
{
	if ( pKeyValues == nullptr )
	{
		return;
	}

	auto keyValue = pKeyValues->FindKeyValue( _T( "Desc" ) );

	if ( keyValue == nullptr )
	{
		DebugWarning( "Load Depth Stencil Desc Fail!!!\n" );
		return;
	}

	for ( auto desc = keyValue->GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		Handle( desc->GetKey(), desc );
	}
}
