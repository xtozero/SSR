#include "stdafx.h"
#include "SamplerStateFactory.h"
#include "IRenderState.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <D3D11.h>
#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* SAMPLER_STATE_DESC_FILE_NAME = _T( "../Script/SamplerStateDesc.txt" );
	constexpr TCHAR* SMAPLER_DESC_HANDLER_KEY_NAME = _T( "SamplerDesc" );

	void SamplerDescHandler( ISamplerStateFactory* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
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
	virtual void Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device* pDevice, const D3D11_SAMPLER_DESC& samplerDesc );
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
};

void CSamplerState::Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type )
{
	assert( type != SHADER_TYPE::NONE );

	if ( pDeviceContext == nullptr )
	{
		return;
	}

	switch ( type )
	{
	case SHADER_TYPE::VS:
		pDeviceContext->VSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::HS:
		pDeviceContext->HSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::DS:
		pDeviceContext->DSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::GS:
		pDeviceContext->GSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::PS:
		pDeviceContext->PSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
		break;
	case SHADER_TYPE::CS:
		pDeviceContext->CSSetSamplers( 0, 1, m_pSamplerState.GetAddressOf( ) );
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
	virtual void Set( ID3D11DeviceContext* ID3D11DeviceContext, const SHADER_TYPE type = NONE ) override;
};

void CNullSamplerState::Set( ID3D11DeviceContext* , const SHADER_TYPE  )
{

}

class CSamplerStateFactory : public ISamplerStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual std::shared_ptr<IRenderState> GetSamplerState( ID3D11Device* pDevice, const String& stateName ) override;
	virtual void AddSamplerDesc( const String& descName, const D3D11_SAMPLER_DESC& newDesc ) override;

	CSamplerStateFactory( );
private:
	void LoadSamplerDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

	std::map<String, std::weak_ptr<IRenderState>> m_samplerState;
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

std::shared_ptr<IRenderState> CSamplerStateFactory::GetSamplerState( ID3D11Device* pDevice, const String& stateName )
{
	auto foundState = m_samplerState.find( stateName );

	if ( foundState != m_samplerState.end( ) )
	{
		if ( !foundState->second.expired( ) )
		{
			return foundState->second.lock( );
		}
	}

	if ( pDevice )
	{
		auto foundDesc = m_samplerStateDesc.find( stateName );

		if ( foundDesc != m_samplerStateDesc.end( ) )
		{
			std::shared_ptr<CSamplerState> newState = std::make_shared<CSamplerState>( );

			if ( newState->Create( pDevice, foundDesc->second ) )
			{
				m_samplerState.emplace( stateName, newState );
				return newState;
			}
		}
	}

	std::shared_ptr<IRenderState> nullState = std::make_shared<CNullSamplerState>( );
	m_samplerState.emplace( _T( "NULL" ), nullState );
	return nullState;
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