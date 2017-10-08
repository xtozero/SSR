#include "stdafx.h"
#include "D3D11RasterizerStateFactory.h"

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
	constexpr TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/RasterizerStateDesc.txt" );
	constexpr TCHAR* RASTERIZER_DESC_HANDLER_KEY_NAME = _T( "RasterizerDesc" );

	void RasterizerDescHandler( CD3D11RasterizerStateFactory* owner, const String&, const KeyValue* keyValue )
	{
		CD3D11_DEFAULT default;
		CD3D11_RASTERIZER_DESC newDesc( default );

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "FillMode" ) )
			{
				newDesc.FillMode = static_cast<D3D11_FILL_MODE>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_FILL_SOLID ));
			}
			else if ( property->GetKey( ) == _T( "CullMode" ) )
			{
				newDesc.CullMode = static_cast<D3D11_CULL_MODE>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_CULL_BACK ));
			}
			else if ( property->GetKey( ) == _T( "FrontCounterClockwise" ) )
			{
				newDesc.FrontCounterClockwise = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "DepthBias" ) )
			{
				newDesc.DepthBias = property->GetValue<int>( );
			}
			else if ( property->GetKey( ) == _T( "DepthBiasClamp" ) )
			{
				newDesc.DepthBiasClamp = property->GetValue<float>( );
			}
			else if ( property->GetKey( ) == _T( "SlopeScaledDepthBias" ) )
			{
				newDesc.SlopeScaledDepthBias = property->GetValue<float>( );
			}
			else if ( property->GetKey( ) == _T( "DepthClipEnable" ) )
			{
				newDesc.DepthClipEnable = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "ScissorEnable" ) )
			{
				newDesc.ScissorEnable = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "MultisampleEnable" ) )
			{
				newDesc.MultisampleEnable = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "AntialiasedLineEnable" ) )
			{
				newDesc.AntialiasedLineEnable = property->GetValue<int>( ) != 0;
			}
		}

		owner->AddRasterizerDesc( keyValue->GetValue( ), newDesc );
	}
}

class CRasterizerState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device* pDevice, const D3D11_RASTERIZER_DESC& rsDesc );

	CRasterizerState( ID3D11DeviceContext* pDeviceContext ) : m_pDeviceContext( pDeviceContext ) { assert( pDeviceContext ); }
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
};

void CRasterizerState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	m_pDeviceContext->RSSetState( m_pRasterizerState.Get() );
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
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	CNullRasterizerState( ID3D11DeviceContext* pDeviceContext ) : m_pDeviceContext( pDeviceContext ) { assert( pDeviceContext ); }
private:
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
};

void CNullRasterizerState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );
	m_pDeviceContext->RSSetState( nullptr );
}

void CD3D11RasterizerStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	auto pKeyValues = KeyValueReader.LoadKeyValueFromFile( RASTERIZER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadRasterizerDesc( pKeyValues.get() );
	}
}

IRenderState* CD3D11RasterizerStateFactory::GetRasterizerState( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& stateName )
{
	auto foundState = m_rasterizerState.find( stateName );

	if ( foundState != m_rasterizerState.end( ) )
	{
		return foundState->second.get();
	}

	if ( pDevice )
	{
		auto foundDesc = m_rasterizerStateDesc.find( stateName );

		if ( foundDesc != m_rasterizerStateDesc.end( ) )
		{
			std::unique_ptr<CRasterizerState> newState = std::make_unique<CRasterizerState>( pDeviceContext );

			if ( newState->Create( pDevice, foundDesc->second ) )
			{
				CRasterizerState* ret = newState.get( );
				m_rasterizerState.emplace( stateName, std::move( newState ) );
				return ret;
			}
		}
	}

	std::unique_ptr<CNullRasterizerState> nullState = std::make_unique<CNullRasterizerState>( pDeviceContext );
	CNullRasterizerState* ret = nullState.get( );
	m_rasterizerState.emplace( _T( "NULL" ), std::move( nullState ) );
	return ret;
}

void CD3D11RasterizerStateFactory::AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC & newDesc )
{
	m_rasterizerStateDesc.emplace( descName, newDesc );
}

CD3D11RasterizerStateFactory::CD3D11RasterizerStateFactory( )
{
	RegisterHandler( RASTERIZER_DESC_HANDLER_KEY_NAME, RasterizerDescHandler );
}

void CD3D11RasterizerStateFactory::LoadRasterizerDesc( KeyValueGroup* pKeyValues )
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
