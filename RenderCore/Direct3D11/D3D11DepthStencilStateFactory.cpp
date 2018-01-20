#include "stdafx.h"
#include "D3D11DepthStencilStateFactory.h"

#include "D3D11RenderStateManager.h"

#include "../CommonRenderer/IRenderState.h"

#include "../../Shared/Util.h"
#include "../../Engine/EnumStringMap.h"
#include "../../Engine/KeyValueReader.h"

#include <d3d11.h>
#include <map>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* DEPTH_STENCIL_DESC_FILE_NAME = _T( "../Script/DepthStencilDesc.txt" );
	constexpr TCHAR* DEPTH_STENCIL_DESC_HANDLER_KEY_NAME = _T( "DepthStencilDesc" );
}

class CDepthStencilState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device& device, const D3D11_DEPTH_STENCIL_DESC& dsDesc );

	CDepthStencilState( CD3D11RenderStateManager& renderStateManager ) : m_renderStateManager( renderStateManager ) { }
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDsState;
	CD3D11RenderStateManager& m_renderStateManager;
};

void CDepthStencilState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	m_renderStateManager.SetDepthStencilState( m_pDsState.Get( ), 1 );
}

bool CDepthStencilState::Create( ID3D11Device& device, const D3D11_DEPTH_STENCIL_DESC & dsDesc )
{
	return SUCCEEDED( device.CreateDepthStencilState( &dsDesc, &m_pDsState ) );
}

class CNullDepthStencilState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	CNullDepthStencilState( CD3D11RenderStateManager& renderStateManager ) : m_renderStateManager( renderStateManager ) { }

private:
	CD3D11RenderStateManager& m_renderStateManager;
};

void CNullDepthStencilState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	m_renderStateManager.SetDepthStencilState( nullptr, 1 );
}

void CD3D11DepthStencilStateFactory::OnDeviceLost( )
{
	m_depthStencilState.clear( );
}

void CD3D11DepthStencilStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( DEPTH_STENCIL_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadDepthStencilDesc( *pKeyValues );
	}
}

IRenderState* CD3D11DepthStencilStateFactory::GetDepthStencilState( ID3D11Device& device, CD3D11RenderStateManager& renderStateManager, const String& stateName )
{
	auto foundState = m_depthStencilState.find( stateName );

	if ( foundState != m_depthStencilState.end( ) )
	{
		return foundState->second.get( );
	}

	auto foundDesc = m_depthStencilDesc.find( stateName );

	if ( foundDesc != m_depthStencilDesc.end( ) )
	{
		std::unique_ptr<CDepthStencilState> newState = std::make_unique<CDepthStencilState>( renderStateManager );

		if ( newState->Create( device, foundDesc->second ) )
		{
			CDepthStencilState* ret = newState.get( );
			m_depthStencilState.emplace( stateName, std::move( newState ) );
			return ret;
		}
	}
	
	std::unique_ptr<CNullDepthStencilState> nullState = std::make_unique<CNullDepthStencilState>( renderStateManager );
	CNullDepthStencilState* ret = nullState.get( );
	m_depthStencilState.emplace( _T( "NULL" ), std::move( nullState ) );
	return ret;
}

void CD3D11DepthStencilStateFactory::AddDepthStencilDesc( const String& descName, const D3D11_DEPTH_STENCIL_DESC & newDesc )
{
	m_depthStencilDesc.emplace( descName, newDesc );
}

void CD3D11DepthStencilStateFactory::LoadDepthStencilDesc( const KeyValue& keyValue )
{
	for ( auto desc = keyValue.GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		CD3D11_DEFAULT default;
		CD3D11_DEPTH_STENCIL_DESC newDesc( default );

		if ( const KeyValue* pDepthEnable = desc->Find( _T( "DepthEnable" ) ) )
		{
			newDesc.DepthEnable = pDepthEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pDepthWriteMask = desc->Find( _T( "DepthWriteMask" ) ) )
		{
			newDesc.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>( GetEnumStringMap( ).GetEnum( pDepthWriteMask->GetValue( ), D3D11_DEPTH_WRITE_MASK_ALL ) );
		}
		
		if ( const KeyValue* pDepthFunc = desc->Find( _T( "DepthFunc" ) ) )
		{
			newDesc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>( GetEnumStringMap( ).GetEnum( pDepthFunc->GetValue( ), D3D11_COMPARISON_LESS ) );
		}
		
		if ( const KeyValue* pStencilEnable = desc->Find( _T( "StencilEnable" ) ) )
		{
			newDesc.StencilEnable = pStencilEnable->GetValue<int>( ) != 0;
		}
		
		if ( const KeyValue* pStencilReadMask = desc->Find( _T( "StencilReadMask" ) ) )
		{
			newDesc.StencilReadMask = static_cast<UINT8>( pStencilReadMask->GetValue<unsigned int>( ) );
		}
		
		if ( const KeyValue* pStencilWriteMask = desc->Find( _T( "StencilWriteMask" ) ) )
		{
			newDesc.StencilWriteMask = static_cast<UINT8>( pStencilWriteMask->GetValue<unsigned int>( ) );
		}
		
		if ( const KeyValue* pStencilFunc = desc->Find( _T( "FrontFace.StencilFunc" ) ) )
		{
			newDesc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>( GetEnumStringMap( ).GetEnum( pStencilFunc->GetValue( ), D3D11_COMPARISON_ALWAYS ) );
		}
		
		if ( const KeyValue* pStencilFunc = desc->Find( _T( "BackFace.StencilFunc" ) ) )
		{
			newDesc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>( GetEnumStringMap( ).GetEnum( pStencilFunc->GetValue( ), D3D11_COMPARISON_ALWAYS ) );
		}
		
		if ( const KeyValue* pStencilDepthFailOp = desc->Find( _T( "FrontFace.StencilDepthFailOp" ) ) )
		{
			newDesc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilDepthFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}
		
		if ( const KeyValue* pStencilDepthFailOp = desc->Find( _T( "BackFace.StencilDepthFailOp" ) ) )
		{
			newDesc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilDepthFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}
		
		if ( const KeyValue* pStencilPassOp = desc->Find( _T( "FrontFace.StencilPassOp" ) ) )
		{
			newDesc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilPassOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}
		
		if ( const KeyValue* pStencilPassOp = desc->Find( _T( "BackFace.StencilPassOp" ) ) )
		{
			newDesc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilPassOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}
		
		if ( const KeyValue* pStencilFailOp = desc->Find( _T( "FrontFace.StencilFailOp" ) ) )
		{
			newDesc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}
		
		if ( const KeyValue* pStencilFailOp = desc->Find( _T( "BackFace.StencilFailOp" ) ) )
		{
			newDesc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>( GetEnumStringMap( ).GetEnum( pStencilFailOp->GetValue( ), D3D11_STENCIL_OP_KEEP ) );
		}

		AddDepthStencilDesc( desc->GetValue( ), newDesc );
	}
}
