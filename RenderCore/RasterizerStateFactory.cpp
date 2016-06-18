#include "stdafx.h"
#include "RasterizerStateFactory.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <D3D11.h>
#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/RasterizerStateDesc.txt" );
	constexpr TCHAR* RASTERIZER_DESC_HANDLER_KEY_NAME = _T( "RasterizerDesc" );

	void RasterizerDescHandler( IRasterizerStateFactory* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
	{
		CD3D11_DEFAULT default;
		CD3D11_RASTERIZER_DESC newDesc( default );

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "FillMode" ) )
			{
				newDesc.FillMode = static_cast<D3D11_FILL_MODE>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_FILL_SOLID ));
			}
			else if ( property->GetKey( ) == _T( "CullMode" ) )
			{
				newDesc.CullMode = static_cast<D3D11_CULL_MODE>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_CULL_BACK ));
			}
			else if ( property->GetKey( ) == _T( "FrontCounterClockwise" ) )
			{
				newDesc.FrontCounterClockwise = property->GetInt( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "DepthBias" ) )
			{
				newDesc.DepthBias = property->GetInt( );
			}
			else if ( property->GetKey( ) == _T( "DepthBiasClamp" ) )
			{
				newDesc.DepthBiasClamp = property->GetFloat( );
			}
			else if ( property->GetKey( ) == _T( "SlopeScaledDepthBias" ) )
			{
				newDesc.SlopeScaledDepthBias = property->GetFloat( );
			}
			else if ( property->GetKey( ) == _T( "DepthClipEnable" ) )
			{
				newDesc.DepthClipEnable = property->GetInt( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "ScissorEnable" ) )
			{
				newDesc.ScissorEnable = property->GetInt( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "MultisampleEnable" ) )
			{
				newDesc.MultisampleEnable = property->GetInt( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "AntialiasedLineEnable" ) )
			{
				newDesc.AntialiasedLineEnable = property->GetInt( ) != 0;
			}
		}

		owner->AddRasterizerDesc( keyValue->GetString( ), newDesc );
	}
}

class CRasterizerStateFactory : public IRasterizerStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState( ID3D11Device* pDevice, const String& stateName ) override;
	virtual void AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC& newDesc ) override;

	CRasterizerStateFactory( );
private:
	void LoadRasterizerDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

	std::map<String, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> m_rasterizerState;
	std::map<String, D3D11_RASTERIZER_DESC> m_rasterizerStateDesc;
};


void CRasterizerStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	auto pKeyValues = KeyValueReader.LoadKeyValueFromFile( RASTERIZER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadRasterizerDesc( pKeyValues );
	}
}

Microsoft::WRL::ComPtr<ID3D11RasterizerState> CRasterizerStateFactory::GetRasterizerState( ID3D11Device* pDevice, const String& stateName )
{
	auto foundState = m_rasterizerState.find( stateName );

	if ( foundState != m_rasterizerState.end( ) )
	{
		return foundState->second;
	}

	if ( pDevice )
	{
		auto foundDesc = m_rasterizerStateDesc.find( stateName );

		if ( foundDesc != m_rasterizerStateDesc.end( ) )
		{
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> newState;

			if ( SUCCEEDED( pDevice->CreateRasterizerState( &foundDesc->second, &newState ) ) )
			{
				m_rasterizerState.emplace( stateName, newState );
				return newState;
			}
		}
	}

	return nullptr;
}

void CRasterizerStateFactory::AddRasterizerDesc( const String & descName, const D3D11_RASTERIZER_DESC & newDesc )
{
	m_rasterizerStateDesc.emplace( descName, newDesc );
}

CRasterizerStateFactory::CRasterizerStateFactory( )
{
	RegisterHandler( RASTERIZER_DESC_HANDLER_KEY_NAME, RasterizerDescHandler );
}

void CRasterizerStateFactory::LoadRasterizerDesc( std::shared_ptr<KeyValueGroup> pKeyValues )
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
		Handle( desc->GetKey( ), desc );
	}
}

std::unique_ptr<IRasterizerStateFactory> CreateRasterizerStateFactory( )
{
	return std::make_unique<CRasterizerStateFactory>( );
}