#include "stdafx.h"
#include "BlendStateFactory.h"
#include "IRenderState.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <D3D11.h>
#include <map>
#include <memory>
#include <wrl/client.h>

struct CD3D_BLEND_DESC
{
	CD3D11_BLEND_DESC m_desc = CD3D11_BLEND_DESC( CD3D11_DEFAULT( ) );
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
};

namespace
{
	constexpr TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/BlendStateDesc.txt" );
	constexpr TCHAR* RASTERIZER_DESC_HANDLER_KEY_NAME = _T( "BlendDesc" );

	void BlendDescHandler( IBlendStateFactory* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
	{
		CD3D_BLEND_DESC newDesc;
		CD3D11_BLEND_DESC& blendDesc = newDesc.m_desc;

		for ( auto property = keyValue->GetChild( ); property != nullptr; property = property->GetNext( ) )
		{
			if ( property->GetKey( ) == _T( "AlphaToCoverageEnable" ) )
			{
				blendDesc.AlphaToCoverageEnable = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "IndependentBlendEnable" ) )
			{
				blendDesc.IndependentBlendEnable = property->GetValue<int>( ) != 0;
			}
			else if ( property->GetKey( ) == _T( "RenderTarget" ) )
			{
				int idx = property->GetValue<int>( );
				
				assert( idx >= 0 && idx < 8 );

				D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[idx];
				for ( auto rtProperty = property->GetChild( ); rtProperty != nullptr; rtProperty = rtProperty->GetNext( ) )
				{
					if ( rtProperty->GetKey( ) == _T( "BlendEnable" ) )
					{
						rtBlendDesc.BlendEnable = rtProperty->GetValue<int>( ) != 0;
					}
					else if ( rtProperty->GetKey( ) == _T( "BlendOp" ) )
					{
						rtBlendDesc.BlendOp = static_cast<D3D11_BLEND_OP>(GetEnumStringMap( ).GetEnum( rtProperty->GetValue( ), D3D11_BLEND_OP_ADD ));
					}
					else if ( rtProperty->GetKey( ) == _T( "BlendOpAlpha" ) )
					{
						rtBlendDesc.BlendOpAlpha = static_cast<D3D11_BLEND_OP>(GetEnumStringMap( ).GetEnum( rtProperty->GetValue( ), D3D11_BLEND_OP_ADD ));
					}
					else if ( rtProperty->GetKey( ) == _T( "DestBlend" ) )
					{
						rtBlendDesc.DestBlend = static_cast<D3D11_BLEND>(GetEnumStringMap( ).GetEnum( rtProperty->GetValue( ), D3D11_BLEND_ZERO ));
					}
					else if ( rtProperty->GetKey( ) == _T( "DestBlendAlpha" ) )
					{
						rtBlendDesc.DestBlendAlpha = static_cast<D3D11_BLEND>(GetEnumStringMap( ).GetEnum( rtProperty->GetValue( ), D3D11_BLEND_ZERO ));
					}
					else if ( rtProperty->GetKey( ) == _T( "SrcBlend" ) )
					{
						rtBlendDesc.SrcBlend = static_cast<D3D11_BLEND>(GetEnumStringMap( ).GetEnum( rtProperty->GetValue( ), D3D11_BLEND_ONE ));
					}
					else if ( rtProperty->GetKey( ) == _T( "SrcBlendAlpha" ) )
					{
						rtBlendDesc.SrcBlendAlpha = static_cast<D3D11_BLEND>(GetEnumStringMap( ).GetEnum( rtProperty->GetValue( ), D3D11_BLEND_ONE ));
					}
					else if ( rtProperty->GetKey( ) == _T( "RenderTargetWriteMask" ) )
					{
						rtBlendDesc.RenderTargetWriteMask = static_cast<UINT8>(GetEnumStringMap( ).GetEnum( rtProperty->GetValue( ), D3D11_COLOR_WRITE_ENABLE_ALL ));
					};
				}
			}
			else if ( property->GetKey( ) == _T( "BlendFactor" ) )
			{
				Stringstream ss( property->GetValue( ) );
				
				for ( auto factor = newDesc.m_blendFactor.begin( ); ss && factor != newDesc.m_blendFactor.end( ); ++factor )
				{
					ss >> *factor;
				}
			}
			else if ( property->GetKey( ) == _T( "SampleMask" ) )
			{
				Stringstream ss( property->GetValue( ) );
				ss >> newDesc.m_sampleMask;
			}
		}

		owner->AddBlendDesc( keyValue->GetValue( ), newDesc );
	}
}

class CBlendState : public IRenderState
{
public:
	virtual void Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device* pDevice, const CD3D_BLEND_DESC& blendDesc );
	void SetBlendFactor( const std::array<float, 4>& blendFactor ) noexcept;
	void SetSampleMask( unsigned int sampleMask ) noexcept { m_sampleMask = sampleMask; }

private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
};

void CBlendState::Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	if ( pDeviceContext )
	{
		pDeviceContext->OMSetBlendState( m_pBlendState.Get( ), m_blendFactor.data(), m_sampleMask );
	}
}

bool CBlendState::Create( ID3D11Device* pDevice, const CD3D_BLEND_DESC& blendDesc )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	HRESULT hr = pDevice->CreateBlendState( &blendDesc.m_desc, &m_pBlendState );
	return SUCCEEDED( hr );
}

void CBlendState::SetBlendFactor( const std::array<float, 4>& blendFactor ) noexcept
{
	m_blendFactor = blendFactor;
}

class CNullBlendState : public IRenderState
{
public:
	virtual void Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type = SHADER_TYPE::NONE ) override;
};

void CNullBlendState::Set( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );

	if ( pDeviceContext )
	{
		pDeviceContext->OMSetBlendState( nullptr, nullptr, D3D11_DEFAULT_SAMPLE_MASK );
	}
}

class CBlendStateFactory : public IBlendStateFactory
{
public:
	virtual void LoadDesc( ) override;
	virtual std::shared_ptr<IRenderState> GetBlendState( ID3D11Device* pDevice, const String& stateName ) override;
	virtual void AddBlendDesc( const String& descName, const CD3D_BLEND_DESC& newDesc ) override;

	CBlendStateFactory( );
private:
	void LoadRasterizerDesc( std::shared_ptr<KeyValueGroup> pKeyValues );

	std::map<String, std::weak_ptr<IRenderState>> m_blendState;
	std::map<String, CD3D_BLEND_DESC> m_blendStateDesc;
};

void CBlendStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	auto pKeyValues = KeyValueReader.LoadKeyValueFromFile( RASTERIZER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadRasterizerDesc( pKeyValues );
	}
}

std::shared_ptr<IRenderState> CBlendStateFactory::GetBlendState( ID3D11Device* pDevice, const String& stateName )
{
	auto foundState = m_blendState.find( stateName );

	if ( foundState != m_blendState.end( ) )
	{
		if ( !foundState->second.expired( ) )
		{
			return foundState->second.lock( );
		}
	}

	if ( pDevice )
	{
		auto foundDesc = m_blendStateDesc.find( stateName );

		if ( foundDesc != m_blendStateDesc.end( ) )
		{
			std::shared_ptr<CBlendState> newState = std::make_shared<CBlendState>( );

			if ( newState->Create( pDevice, foundDesc->second ) )
			{
				m_blendState.emplace( stateName, newState );
				return newState;
			}
		}
	}

	std::shared_ptr<IRenderState> nullState = std::make_shared<CNullBlendState>( );
	m_blendState.emplace( _T( "NULL" ), nullState );
	return nullState;
}

void CBlendStateFactory::AddBlendDesc( const String & descName, const CD3D_BLEND_DESC& newDesc )
{
	m_blendStateDesc.emplace( descName, newDesc );
}

CBlendStateFactory::CBlendStateFactory( )
{
	RegisterHandler( RASTERIZER_DESC_HANDLER_KEY_NAME, BlendDescHandler );
}

void CBlendStateFactory::LoadRasterizerDesc( std::shared_ptr<KeyValueGroup> pKeyValues )
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

std::unique_ptr<IBlendStateFactory> CreateBlendStateFactory( )
{
	return std::make_unique<CBlendStateFactory>( );
}