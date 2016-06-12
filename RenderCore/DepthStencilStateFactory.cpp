#include "stdafx.h"
#include "DepthStencilStateFactory.h"

#include "../Shared/Util.h"
#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"

#include <d3d11.h>
#include <map>
#include <wrl/client.h>

namespace
{
	const TCHAR* DEPTH_STENCIL_DESC_FILE_NAME = _T( "../Script/DepthStencilDesc.txt" );
	const TCHAR* DEPTH_STENCIL_DESC_HANDLER_KEY_NAME = _T( "DepthStencilDesc" );

	void DepthStencilDescHandler( IDepthStencilStateFactory* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
	{
		CD3D11_DEFAULT default;
		CD3D11_DEPTH_STENCIL_DESC newDesc( default );

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "DepthEnable" ) )
			{
				newDesc.DepthEnable = property->GetInt( ) != 0;
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
				newDesc.StencilEnable = property->GetInt( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "StencilReadMask" ) )
			{
				newDesc.StencilReadMask = static_cast<UINT8>(property->GetInt( ));
			}
			else if ( property->GetKey( ) == _T( "StencilWriteMask" ) )
			{
				newDesc.StencilWriteMask = static_cast<UINT8>(property->GetInt( ));
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

class CDepthStencilStateFactory : public IDepthStencilStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState( ID3D11Device* pDevice, const String& stateName ) override;
	virtual void AddDepthStencilDesc( const String & descName, const D3D11_DEPTH_STENCIL_DESC & newDesc ) override;

	CDepthStencilStateFactory( );
private:
	void LoadDepthStencilDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

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