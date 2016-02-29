#include "stdafx.h"
#include "SamplerStateFactory.h"

#include "StateFactoryHelpUtil.h"

#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <D3D11.h>
#include <map>
#include <memory>

namespace
{
	const TCHAR* SAMPLER_STATE_DESC_FILE_NAME = _T( "../Script/SamplerStateDesc.txt" );
}

class CSamplerStateFactory : public ISamplerStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState( ID3D11Device* pDevice, const String& stateName ) override;

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
		CD3D11_SAMPLER_DESC newDesc;

		for ( auto property = desc->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "Filter" ) )
			{
				newDesc.Filter = TranslateFilter( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "AddressU" ) )
			{
				newDesc.AddressU = TranslateTextrueAddressMode( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "AddressV" ) )
			{
				newDesc.AddressV = TranslateTextrueAddressMode( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "AddressW" ) )
			{
				newDesc.AddressW = TranslateTextrueAddressMode( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "MipLODBias" ) )
			{
				newDesc.MipLODBias = property->GetFloat( );
			}
			else if ( property->GetKey( ) == _T( "MaxAnisotropy" ) )
			{
				newDesc.MaxAnisotropy = static_cast<UINT>( property->GetInt( ) );
			}
			else if ( property->GetKey( ) == _T( "ComparisonFunc" ) )
			{
				newDesc.ComparisonFunc = TranslateComparisonFunc( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "BorderColor" ) )
			{
				std::vector<String> prarams;
				UTIL::Split( property->GetString( ), prarams, _T( ' ' ) );

				if ( prarams.size( ) <= 4 )
				{
					FOR_EACH_VEC( prarams, i )
					{
						newDesc.BorderColor[std::distance( prarams.begin( ), i )] = static_cast<float>( _ttof( i->c_str() ) );
					}
				}
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

		m_samplerStateDesc.emplace( desc->GetKey( ), newDesc );
	}
}

std::unique_ptr<ISamplerStateFactory> CreateSamplerStateFactory( )
{
	return std::make_unique<CSamplerStateFactory>( );
}