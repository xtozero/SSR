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
}

class CRasterizerState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device& device, const D3D11_RASTERIZER_DESC& rsDesc );

	CRasterizerState( ID3D11DeviceContext& deviceContext ) : m_deviceContext( deviceContext ) { }
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	ID3D11DeviceContext& m_deviceContext;
};

void CRasterizerState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	m_deviceContext.RSSetState( m_pRasterizerState.Get() );
}

bool CRasterizerState::Create( ID3D11Device& device, const D3D11_RASTERIZER_DESC& rsDesc )
{
	return SUCCEEDED( device.CreateRasterizerState( &rsDesc, &m_pRasterizerState ) );
}

class CNullRasterizerState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	CNullRasterizerState( ID3D11DeviceContext& deviceContext ) : m_deviceContext( deviceContext ) { }
private:
	ID3D11DeviceContext& m_deviceContext;
};

void CNullRasterizerState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );
	m_deviceContext.RSSetState( nullptr );
}

void CD3D11RasterizerStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( RASTERIZER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadRasterizerDesc( *pKeyValues );
	}
}

IRenderState* CD3D11RasterizerStateFactory::GetRasterizerState( ID3D11Device& device, ID3D11DeviceContext& deviceContext, const String& stateName )
{
	auto foundState = m_rasterizerState.find( stateName );

	if ( foundState != m_rasterizerState.end( ) )
	{
		return foundState->second.get();
	}

	auto foundDesc = m_rasterizerStateDesc.find( stateName );

	if ( foundDesc != m_rasterizerStateDesc.end( ) )
	{
		std::unique_ptr<CRasterizerState> newState = std::make_unique<CRasterizerState>( deviceContext );

		if ( newState->Create( device, foundDesc->second ) )
		{
			CRasterizerState* ret = newState.get( );
			m_rasterizerState.emplace( stateName, std::move( newState ) );
			return ret;
		}
	}

	std::unique_ptr<CNullRasterizerState> nullState = std::make_unique<CNullRasterizerState>( deviceContext );
	CNullRasterizerState* ret = nullState.get( );
	m_rasterizerState.emplace( _T( "NULL" ), std::move( nullState ) );
	return ret;
}

void CD3D11RasterizerStateFactory::AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC & newDesc )
{
	m_rasterizerStateDesc.emplace( descName, newDesc );
}

void CD3D11RasterizerStateFactory::LoadRasterizerDesc( const KeyValue& keyValue )
{
	for ( auto desc = keyValue.GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		CD3D11_DEFAULT default;
		CD3D11_RASTERIZER_DESC newDesc( default );

		if ( const KeyValue* pFillMode = desc->Find( _T( "FillMode" ) ) )
		{
			newDesc.FillMode = static_cast<D3D11_FILL_MODE>( GetEnumStringMap( ).GetEnum( pFillMode->GetValue( ), D3D11_FILL_SOLID ) );
		}
		
		if ( const KeyValue* pCullMode = desc->Find( _T( "CullMode" ) ) )
		{
			newDesc.CullMode = static_cast<D3D11_CULL_MODE>( GetEnumStringMap( ).GetEnum( pCullMode->GetValue( ), D3D11_CULL_BACK ) );
		}
		
		if ( const KeyValue* pFrontCounterClockwise = desc->Find( _T( "FrontCounterClockwise" ) ) )
		{
			newDesc.FrontCounterClockwise = pFrontCounterClockwise->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pDepthBias = desc->Find( _T( "DepthBias" ) ) )
		{
			newDesc.DepthBias = pDepthBias->GetValue<int>( );
		}
		
		if ( const KeyValue* pDepthBiasClamp = desc->Find( _T( "DepthBiasClamp" ) ) )
		{
			newDesc.DepthBiasClamp = pDepthBiasClamp->GetValue<float>( );
		}
		
		if ( const KeyValue* pSlopeScaledDepthBias = desc->Find( _T( "SlopeScaledDepthBias" ) ) )
		{
			newDesc.SlopeScaledDepthBias = pSlopeScaledDepthBias->GetValue<float>( );
		}
		
		if ( const KeyValue* pDepthClipEnable = desc->Find( _T( "DepthClipEnable" ) ) )
		{
			newDesc.DepthClipEnable = pDepthClipEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pScissorEnable = desc->Find( _T( "ScissorEnable" ) ) )
		{
			newDesc.ScissorEnable = pScissorEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pMultisampleEnable = desc->Find( _T( "MultisampleEnable" ) ) )
		{
			newDesc.MultisampleEnable = pMultisampleEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pAntialiasedLineEnable = desc->Find( _T( "AntialiasedLineEnable" ) ) )
		{
			newDesc.AntialiasedLineEnable = pAntialiasedLineEnable->GetValue<int>( ) != 0;
		}

		AddRasterizerDesc( desc->GetValue( ), newDesc );
	}
}
