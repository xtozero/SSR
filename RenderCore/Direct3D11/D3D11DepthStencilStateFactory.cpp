#include "stdafx.h"
#include "D3D11DepthStencilStateFactory.h"
#include "../IRenderState.h"

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

	void DepthStencilDescHandler( CD3D11DepthStencilStateFactory* owner, const String&, const KeyValue* keyValue )
	{
		CD3D11_DEFAULT default;
		CD3D11_DEPTH_STENCIL_DESC newDesc( default );

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "DepthEnable" ) )
			{
				newDesc.DepthEnable = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "DepthWriteMask" ) )
			{
				newDesc.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_DEPTH_WRITE_MASK_ALL ));
			}
			else if ( property->GetKey( ) == _T( "DepthFunc" ) )
			{
				newDesc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_COMPARISON_LESS ));
			}
			else if ( property->GetKey( ) == _T( "StencilEnable" ) )
			{
				newDesc.StencilEnable = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "StencilReadMask" ) )
			{
				newDesc.StencilReadMask = static_cast<UINT8>(property->GetValue<unsigned int>( ));
			}
			else if ( property->GetKey( ) == _T( "StencilWriteMask" ) )
			{
				newDesc.StencilWriteMask = static_cast<UINT8>(property->GetValue<unsigned int>( ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilFunc" ) )
			{
				newDesc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_COMPARISON_ALWAYS ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilFunc" ) )
			{
				newDesc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_COMPARISON_ALWAYS ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilDepthFailOp" ) )
			{
				newDesc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilDepthFailOp" ) )
			{
				newDesc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilPassOp" ) )
			{
				newDesc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilPassOp" ) )
			{
				newDesc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilFailOp" ) )
			{
				newDesc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilFailOp" ) )
			{
				newDesc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetValue( ), D3D11_STENCIL_OP_KEEP ));
			}
		}

		owner->AddDepthStencilDesc( keyValue->GetValue( ), newDesc );
	}
}

class CDepthStencilState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC& dsDesc );

	CDepthStencilState( ID3D11DeviceContext* pDeviceContext ) : m_pDeviceContext( pDeviceContext ) { assert( m_pDeviceContext ); }
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDsState;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
};

void CDepthStencilState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	m_pDeviceContext->OMSetDepthStencilState( m_pDsState.Get( ), 1 );
}

bool CDepthStencilState::Create( ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC & dsDesc )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	HRESULT hr = pDevice->CreateDepthStencilState( &dsDesc, &m_pDsState);
	return SUCCEEDED( hr );
}

class CNullDepthStencilState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	CNullDepthStencilState( ID3D11DeviceContext* pDeviceContext ) : m_pDeviceContext( pDeviceContext ) { assert( m_pDeviceContext ); }

private:
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
};

void CNullDepthStencilState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	m_pDeviceContext->OMSetDepthStencilState( nullptr, 1 );
}

void CD3D11DepthStencilStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValueGroupImpl> pKeyValues = KeyValueReader.LoadKeyValueFromFile( DEPTH_STENCIL_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadDepthStencilDesc( pKeyValues.get() );
	}
}

IRenderState* CD3D11DepthStencilStateFactory::GetDepthStencilState( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& stateName )
{
	auto foundState = m_depthStencilState.find( stateName );

	if ( foundState != m_depthStencilState.end( ) )
	{
		return foundState->second.get( );
	}

	if ( pDevice )
	{
		auto foundDesc = m_depthStencilDesc.find( stateName );

		if ( foundDesc != m_depthStencilDesc.end( ) )
		{
			std::unique_ptr<CDepthStencilState> newState = std::make_unique<CDepthStencilState>( pDeviceContext );

			if ( newState->Create( pDevice, foundDesc->second ) )
			{
				CDepthStencilState* ret = newState.get( );
				m_depthStencilState.emplace( stateName, std::move( newState ) );
				return ret;
			}
		}
	}
	
	std::unique_ptr<CNullDepthStencilState> nullState = std::make_unique<CNullDepthStencilState>( pDeviceContext );
	CNullDepthStencilState* ret = nullState.get( );
	m_depthStencilState.emplace( _T( "NULL" ), std::move( nullState ) );
	return ret;
}

void CD3D11DepthStencilStateFactory::AddDepthStencilDesc( const String& descName, const D3D11_DEPTH_STENCIL_DESC & newDesc )
{
	m_depthStencilDesc.emplace( descName, newDesc );
}

CD3D11DepthStencilStateFactory::CD3D11DepthStencilStateFactory( )
{
	RegisterHandler( DEPTH_STENCIL_DESC_HANDLER_KEY_NAME, DepthStencilDescHandler );
}

void CD3D11DepthStencilStateFactory::LoadDepthStencilDesc( const KeyValueGroup* pKeyValues )
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
