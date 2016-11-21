#include "stdafx.h"
#include "RasterizerStateFactory.h"
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

class CRasterizerState : public IRenderState
{
public:
	virtual void Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC& rsDesc );
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
};

void CRasterizerState::Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	if ( pDeviceContext )
	{
		pDeviceContext->RSSetState( m_pRasterizerState.Get() );
	}
}

bool CRasterizerState::Create( ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC& rsDesc )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	HRESULT hr = pDevice->CreateRasterizerState( &rsDesc, &m_pRasterizerState );
	return SUCCEEDED( hr );
}

class CNullRasterizerState : public IRenderState
{
public:
	virtual void Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type = SHADER_TYPE::NONE ) override;
};

void CNullRasterizerState::Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	if ( pDeviceContext )
	{
		pDeviceContext->RSSetState( nullptr );
	}
}

class CRasterizerStateFactory : public IRasterizerStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual std::shared_ptr<IRenderState> GetRasterizerState( ID3D11Device* pDevice, const String& stateName ) override;
	virtual void AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC& newDesc ) override;

	CRasterizerStateFactory( );
private:
	void LoadRasterizerDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

	std::map<String, std::weak_ptr<IRenderState>> m_rasterizerState;
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

std::shared_ptr<IRenderState> CRasterizerStateFactory::GetRasterizerState( ID3D11Device* pDevice, const String& stateName )
{
	auto foundState = m_rasterizerState.find( stateName );

	if ( foundState != m_rasterizerState.end( ) )
	{
		if ( !foundState->second.expired( ) )
		{
			return foundState->second.lock( );
		}
	}

	if ( pDevice )
	{
		auto foundDesc = m_rasterizerStateDesc.find( stateName );

		if ( foundDesc != m_rasterizerStateDesc.end( ) )
		{
			std::shared_ptr<CRasterizerState> newState = std::make_shared<CRasterizerState>();

			if ( newState->Create( pDevice, foundDesc->second ) )
			{
				m_rasterizerState.emplace( stateName, newState );
				return newState;
			}
		}
	}

	std::shared_ptr<IRenderState> nullState = std::make_shared<CNullRasterizerState>( );
	m_rasterizerState.emplace( _T( "NULL" ), nullState );
	return nullState;
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