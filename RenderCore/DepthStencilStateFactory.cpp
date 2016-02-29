#include "stdafx.h"
#include "DepthStencilStateFactory.h"

#include "StateFactoryHelpUtil.h"

#include "../Shared/Util.h"
#include "../Engine/KeyValueReader.h"

#include <d3d11.h>
#include <map>

namespace
{
	const TCHAR* DEPTH_STENCIL_DESC_FILE_NAME = _T( "../Script/DepthStencilDesc.txt" );
}

class CDepthStencilStateFactory : public IDepthStencilStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState( ID3D11Device* pDevice, const String& stateName ) override;

private:
	void LoadDepthStencilDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

private:
	std::map<String, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>> m_depthStencilState;
	std::map<String, D3D11_DEPTH_STENCIL_DESC> m_depthStencilDesc;
};

void CDepthStencilStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	auto pKeyValues = KeyValueReader.LoadKeyValueFromFile( DEPTH_STENCIL_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadDepthStencilDesc( pKeyValues );
	}
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CDepthStencilStateFactory::GetDepthStencilState( ID3D11Device* pDevice, const String& stateName )
{
	auto foundState = m_depthStencilState.find( stateName );

	if ( foundState != m_depthStencilState.end( ) )
	{
		return foundState->second;
	}

	if ( pDevice )
	{
		auto foundDesc = m_depthStencilDesc.find( stateName );

		if ( foundDesc != m_depthStencilDesc.end( ) )
		{
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> newState;

			if ( SUCCEEDED( pDevice->CreateDepthStencilState( &foundDesc->second, &newState ) ) )
			{
				m_depthStencilState.emplace( stateName, newState );
				return newState;
			}
		}
	}
	
	return nullptr;
}

void CDepthStencilStateFactory::LoadDepthStencilDesc( std::shared_ptr<KeyValueGroup> pKeyValues )
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
		CD3D11_DEPTH_STENCIL_DESC newDesc;

		for ( auto property = desc->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "DepthEnable" ) )
			{
				newDesc.DepthEnable = property->GetInt( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "DepthWriteMask" ) )
			{
				newDesc.DepthWriteMask = TranslateDepthWriteMask( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "DepthFunc" ) )
			{
				newDesc.DepthFunc = TranslateComparisonFunc( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "StencilEnable" ) )
			{
				newDesc.StencilEnable = property->GetInt( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "StencilReadMask" ) )
			{
				newDesc.StencilReadMask = static_cast<UINT8>( property->GetInt( ) );
			}
			else if ( property->GetKey( ) == _T( "StencilWriteMask" ) )
			{
				newDesc.StencilWriteMask = static_cast<UINT8>( property->GetInt( ) );
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilFunc" ) )
			{
				newDesc.FrontFace.StencilFunc = TranslateComparisonFunc( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilFunc" ) )
			{
				newDesc.BackFace.StencilFunc = TranslateComparisonFunc( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilDepthFailOp" ) )
			{
				newDesc.FrontFace.StencilDepthFailOp = TranslateStencilOP( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilDepthFailOp" ) )
			{
				newDesc.BackFace.StencilDepthFailOp = TranslateStencilOP( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilPassOp" ) )
			{
				newDesc.FrontFace.StencilPassOp = TranslateStencilOP( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilPassOp" ) )
			{
				newDesc.BackFace.StencilPassOp = TranslateStencilOP( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilFailOp" ) )
			{
				newDesc.FrontFace.StencilFailOp = TranslateStencilOP( property->GetString( ) );
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilFailOp" ) )
			{
				newDesc.BackFace.StencilFailOp = TranslateStencilOP( property->GetString( ) );
			}
		}

		m_depthStencilDesc.emplace( desc->GetKey( ), newDesc );
	}
}

std::unique_ptr<IDepthStencilStateFactory> CreateDepthStencailStateFactory( )
{
	return std::make_unique<CDepthStencilStateFactory>();
}