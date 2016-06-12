#include "stdafx.h"
#include "SamplerStateFactory.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <D3D11.h>
#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	const TCHAR* SAMPLER_STATE_DESC_FILE_NAME = _T( "../Script/SamplerStateDesc.txt" );
	const TCHAR* SMAPLER_DESC_HANDLER_KEY_NAME = _T( "SamplerDesc" );

	void SamplerDescHandler( ISamplerStateFactory* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
	{
		CD3D11_DEFAULT default;
		CD3D11_SAMPLER_DESC newDesc( default );

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "Filter" ) )
			{
				newDesc.Filter = static_cast<D3D11_FILTER>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_FILTER_MIN_MAG_MIP_LINEAR ));
			}
			else if ( property->GetKey( ) == _T( "AddressU" ) )
			{
				newDesc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_TEXTURE_ADDRESS_CLAMP ));
			}
			else if ( property->GetKey( ) == _T( "AddressV" ) )
			{
				newDesc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_TEXTURE_ADDRESS_CLAMP ));
			}
			else if ( property->GetKey( ) == _T( "AddressW" ) )
			{
				newDesc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_TEXTURE_ADDRESS_CLAMP ));
			}
			else if ( property->GetKey( ) == _T( "MipLODBias" ) )
			{
				newDesc.MipLODBias = property->GetFloat( );
			}
			else if ( property->GetKey( ) == _T( "MaxAnisotropy" ) )
			{
				newDesc.MaxAnisotropy = static_cast<UINT>(property->GetInt( ));
			}
			else if ( property->GetKey( ) == _T( "ComparisonFunc" ) )
			{
				newDesc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_COMPARISON_LESS ));
			}
			else if ( property->GetKey( ) == _T( "BorderColor" ) )
			{
				Stringstream sStream;
				sStream << property->GetString( );

				sStream >> newDesc.BorderColor[0] >> newDesc.BorderColor[1] >> newDesc.BorderColor[2] >> newDesc.BorderColor[3];
			}
			else if ( property->GetKey( ) == _T( "MinLOD" ) )
			{
				newDesc.MinLOD = property->GetFloat( );
			}
			else if ( property->GetKey( ) == _T( "MaxLOD" ) )
			{
				newDesc.MaxLOD = property->GetFloat( );
			}
		}

		owner->AddSamplerDesc( keyValue->GetString(), newDesc );
	}
}

class CSamplerStateFactory : public ISamplerStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState( ID3D11Device* pDevice, const String& stateName ) override;
	virtual void AddSamplerDesc( const String& descName, const D3D11_SAMPLER_DESC& newDesc ) override;

	CSamplerStateFactory( );
private:
	void LoadSamplerDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

	std::map<String, Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_samplerState;
	std::map<String, D3D11_SAMPLER_DESC> m_samplerStateDesc;
};


void CSamplerStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	auto pKeyValues = KeyValueReader.LoadKeyValueFromFile( SAMPLER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadSamplerDesc( pKeyValues );
	}
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> CSamplerStateFactory::GetSamplerState( ID3D11Device* pDevice, const String& stateName )
{
	auto foundState = m_samplerState.find( stateName );

	if ( foundState != m_samplerState.end( ) )
	{
		return foundState->second;
	}

	if ( pDevice )
	{
		auto foundDesc = m_samplerStateDesc.find( stateName );

		if ( foundDesc != m_samplerStateDesc.end( ) )
		{
			Microsoft::WRL::ComPtr<ID3D11SamplerState> newState;

			if ( SUCCEEDED( pDevice->CreateSamplerState( &foundDesc->second, &newState ) ) )
			{
				m_samplerState.emplace( stateName, newState );
				return newState;
			}
		}
	}

	return nullptr;
}

void CSamplerStateFactory::AddSamplerDesc( const String& descName, const D3D11_SAMPLER_DESC& newDesc )
{
	m_samplerStateDesc.emplace( descName, newDesc );
}

CSamplerStateFactory::CSamplerStateFactory( )
{
	RegisterHandler( SMAPLER_DESC_HANDLER_KEY_NAME, SamplerDescHandler );
}

void CSamplerStateFactory::LoadSamplerDesc( std::shared_ptr<KeyValueGroup> pKeyValues )
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

std::unique_ptr<ISamplerStateFactory> CreateSamplerStateFactory( )
{
	return std::make_unique<CSamplerStateFactory>( );
}