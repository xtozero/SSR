#include "stdafx.h"
#include "DepthStencilStateFactory.h"
#include "IRenderState.h"

#include "../Shared/Util.h"
#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"

#include <d3d11.h>
#include <map>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* DEPTH_STENCIL_DESC_FILE_NAME = _T( "../Script/DepthStencilDesc.txt" );
	constexpr TCHAR* DEPTH_STENCIL_DESC_HANDLER_KEY_NAME = _T( "DepthStencilDesc" );

	void DepthStencilDescHandler( IDepthStencilStateFactory* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
	{
		CD3D11_DEFAULT default;
		CD3D11_DEPTH_STENCIL_DESC newDesc( default );

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "DepthEnable" ) )
			{
				newDesc.DepthEnable = property->Get<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "DepthWriteMask" ) )
			{
				newDesc.DepthWriteMask = static_cast<D3D11_DEPTH_WRITE_MASK>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_DEPTH_WRITE_MASK_ALL ));
			}
			else if ( property->GetKey( ) == _T( "DepthFunc" ) )
			{
				newDesc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_COMPARISON_LESS ));
			}
			else if ( property->GetKey( ) == _T( "StencilEnable" ) )
			{
				newDesc.StencilEnable = property->Get<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "StencilReadMask" ) )
			{
				newDesc.StencilReadMask = static_cast<UINT8>(property->Get<unsigned int>( ));
			}
			else if ( property->GetKey( ) == _T( "StencilWriteMask" ) )
			{
				newDesc.StencilWriteMask = static_cast<UINT8>(property->Get<unsigned int>( ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilFunc" ) )
			{
				newDesc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_COMPARISON_ALWAYS ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilFunc" ) )
			{
				newDesc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_COMPARISON_ALWAYS ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilDepthFailOp" ) )
			{
				newDesc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilDepthFailOp" ) )
			{
				newDesc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilPassOp" ) )
			{
				newDesc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilPassOp" ) )
			{
				newDesc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "FrontFace.StencilFailOp" ) )
			{
				newDesc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_STENCIL_OP_KEEP ));
			}
			else if ( property->GetKey( ) == _T( "BackFace.StencilFailOp" ) )
			{
				newDesc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(GetEnumStringMap( ).GetEnum( property->GetString( ), D3D11_STENCIL_OP_KEEP ));
			}
		}

		owner->AddDepthStencilDesc( keyValue->GetString( ), newDesc );
	}
}

class CDepthStencilState : public IRenderState
{
public:
	virtual void Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device* pDevice, const D3D11_DEPTH_STENCIL_DESC& dsDesc );
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDsState;
};

void CDepthStencilState::Set( ID3D11DeviceContext * pDeviceContext, const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	if ( pDeviceContext )
	{
		pDeviceContext->OMSetDepthStencilState( m_pDsState.Get( ), 1 );
	}
}

bool CDepthStencilState::Create( ID3D11Device * pDevice, const D3D11_DEPTH_STENCIL_DESC & dsDesc )
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
	virtual void Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type = SHADER_TYPE::NONE ) override;
};

void CNullDepthStencilState::Set( ID3D11DeviceContext * pDeviceContext, const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	if ( pDeviceContext )
	{
		pDeviceContext->OMSetDepthStencilState( nullptr, 1 );
	}
}

class CDepthStencilStateFactory : public IDepthStencilStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual std::shared_ptr<IRenderState> GetDepthStencilState( ID3D11Device* pDevice, const String& stateName ) override;
	virtual void AddDepthStencilDesc( const String & descName, const D3D11_DEPTH_STENCIL_DESC & newDesc ) override;

	CDepthStencilStateFactory( );
private:
	void LoadDepthStencilDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

	std::map<String, std::weak_ptr<IRenderState>> m_depthStencilState;
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

std::shared_ptr<IRenderState> CDepthStencilStateFactory::GetDepthStencilState( ID3D11Device* pDevice, const String& stateName )
{
	auto foundState = m_depthStencilState.find( stateName );

	if ( foundState != m_depthStencilState.end( ) )
	{
		if ( !foundState->second.expired( ) )
		{
			return foundState->second.lock( );
		}
	}

	if ( pDevice )
	{
		auto foundDesc = m_depthStencilDesc.find( stateName );

		if ( foundDesc != m_depthStencilDesc.end( ) )
		{
			std::shared_ptr<CDepthStencilState> newState = std::make_shared<CDepthStencilState>();

			if ( newState->Create( pDevice, foundDesc->second ) )
			{
				m_depthStencilState.emplace( stateName, newState );
				return newState;
			}
		}
	}
	
	std::shared_ptr<CNullDepthStencilState> nullState = std::make_shared<CNullDepthStencilState>( );
	m_depthStencilState.emplace( _T( "NULL" ), nullState );
	return nullState;
}

void CDepthStencilStateFactory::AddDepthStencilDesc( const String & descName, const D3D11_DEPTH_STENCIL_DESC & newDesc )
{
	m_depthStencilDesc.emplace( descName, newDesc );
}

CDepthStencilStateFactory::CDepthStencilStateFactory( )
{
	RegisterHandler( DEPTH_STENCIL_DESC_HANDLER_KEY_NAME, DepthStencilDescHandler );
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
		Handle( desc->GetKey( ), desc );
	}
}

std::unique_ptr<IDepthStencilStateFactory> CreateDepthStencailStateFactory( )
{
	return std::make_unique<CDepthStencilStateFactory>();
}
