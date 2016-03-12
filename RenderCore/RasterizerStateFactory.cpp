#include "stdafx.h"
#include "RasterizerStateFactory.h"

#include "StateFactoryHelpUtil.h"

#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <D3D11.h>
#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	const TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/RasterizerStateDesc.txt" );
}

class CRasterizerStateFactory : public IRasterizerStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState( ID3D11Device* pDevice, const String& stateName ) override;

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
		CD3D11_DEFAULT default;
		CD3D11_RASTERIZER_DESC newDesc( default );

		for ( auto property = desc->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "FillMode" ) )
			{
				newDesc.FillMode = TranslateFillMode( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "CullMode" ) )
			{
				newDesc.CullMode = TranslateCullMode( property->GetString( ) );
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

		m_rasterizerStateDesc.emplace( desc->GetKey( ), newDesc );
	}
}

std::unique_ptr<IRasterizerStateFactory> CreateRasterizerStateFactory( )
{
	return std::make_unique<CRasterizerStateFactory>( );
}