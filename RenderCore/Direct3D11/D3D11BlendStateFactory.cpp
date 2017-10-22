#include "stdafx.h"
#include "D3D11BlendStateFactory.h"

#include "../CommonRenderer/IRenderState.h"

#include "../../Engine/EnumStringMap.h"
#include "../../Engine/KeyValueReader.h"
#include "../../Shared/Util.h"

#include <map>
#include <memory>
#include <wrl/client.h>

namespace
{
	constexpr TCHAR* RASTERIZER_STATE_DESC_FILE_NAME = _T( "../Script/BlendStateDesc.txt" );
	constexpr TCHAR* RASTERIZER_DESC_HANDLER_KEY_NAME = _T( "BlendDesc" );
}

class CBlendState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	bool Create( ID3D11Device& device, const CD3D_BLEND_DESC& blendDesc );
	void SetBlendFactor( const std::array<float, 4>& blendFactor ) noexcept;
	void SetSampleMask( unsigned int sampleMask ) noexcept { m_sampleMask = sampleMask; }

	CBlendState( ID3D11DeviceContext& deviceContext ) : m_deviceContext( deviceContext ) {}
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
	ID3D11DeviceContext& m_deviceContext;
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
};

void CBlendState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );
	m_deviceContext.OMSetBlendState( m_pBlendState.Get( ), m_blendFactor.data(), m_sampleMask );
}

bool CBlendState::Create( ID3D11Device& device, const CD3D_BLEND_DESC& blendDesc )
{
	return SUCCEEDED( device.CreateBlendState( &blendDesc.m_desc, &m_pBlendState ) );
}

void CBlendState::SetBlendFactor( const std::array<float, 4>& blendFactor ) noexcept
{
	m_blendFactor = blendFactor;
}

class CNullBlendState : public IRenderState
{
public:
	virtual void Set( const SHADER_TYPE type = SHADER_TYPE::NONE ) override;

	CNullBlendState( ID3D11DeviceContext& deviceContext ) : m_deviceContext( deviceContext ) { }
private:
	ID3D11DeviceContext& m_deviceContext;
};

void CNullBlendState::Set( const SHADER_TYPE type )
{
	assert( type == SHADER_TYPE::NONE );
	m_deviceContext.OMSetBlendState( nullptr, nullptr, D3D11_DEFAULT_SAMPLE_MASK );
}

void CD3D11BlendStateFactory::LoadDesc( )
{
	CKeyValueReader KeyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = KeyValueReader.LoadKeyValueFromFile( RASTERIZER_STATE_DESC_FILE_NAME );

	if ( pKeyValues )
	{
		LoadRasterizerDesc( *pKeyValues );
	}
}

IRenderState* CD3D11BlendStateFactory::GetBlendState( ID3D11Device& device, ID3D11DeviceContext& deviceContext, const String& stateName )
{
	auto foundState = m_blendState.find( stateName );

	if ( foundState != m_blendState.end( ) )
	{
		return foundState->second.get( );
	}

	auto foundDesc = m_blendStateDesc.find( stateName );

	if ( foundDesc != m_blendStateDesc.end( ) )
	{
		std::unique_ptr<CBlendState> newState = std::make_unique<CBlendState>( deviceContext );

		if ( newState->Create( device, foundDesc->second ) )
		{
			CBlendState* ret = newState.get( );
			m_blendState.emplace( stateName, std::move( newState ) );
			return ret;
		}
	}

	std::unique_ptr<CNullBlendState> nullState = std::make_unique<CNullBlendState>( deviceContext );
	CNullBlendState* ret = nullState.get( );
	m_blendState.emplace( _T( "NULL" ), std::move( nullState ) );
	return ret;
}

void CD3D11BlendStateFactory::LoadRasterizerDesc( const KeyValue& keyValue )
{
	for ( auto desc = keyValue.GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		CD3D_BLEND_DESC newDesc;
		CD3D11_BLEND_DESC& blendDesc = newDesc.m_desc;

		if ( const KeyValue* pAlphaToCoverageEnable = desc->Find( _T( "AlphaToCoverageEnable" ) ) )
		{
			blendDesc.AlphaToCoverageEnable = pAlphaToCoverageEnable->GetValue<int>( ) != 0;
		}
		else if ( const KeyValue* pIndependentBlendEnable = desc->Find( _T( "IndependentBlendEnable" ) ) )
		{
			blendDesc.IndependentBlendEnable = pIndependentBlendEnable->GetValue<int>( ) != 0;
		}
		else if ( const KeyValue* pRenderTarget = desc->Find( _T( "RenderTarget" ) ) )
		{
			int idx = pRenderTarget->GetValue<int>( );

			assert( idx >= 0 && idx < 8 );

			D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[idx];
			
			if ( const KeyValue* pBlendEnable = pRenderTarget->Find( _T( "BlendEnable" ) ) )
			{
				rtBlendDesc.BlendEnable = pBlendEnable->GetValue<int>( ) != 0;
			}
			
			if ( const KeyValue* pBlendOp = pRenderTarget->Find( _T( "BlendOp" ) ) )
			{
				rtBlendDesc.BlendOp = static_cast<D3D11_BLEND_OP>( GetEnumStringMap( ).GetEnum( pBlendOp->GetValue( ), D3D11_BLEND_OP_ADD ) );
			}
			
			if ( const KeyValue* pBlendOpAlpha = pRenderTarget->Find( _T( "BlendOpAlpha" ) ) )
			{
				rtBlendDesc.BlendOpAlpha = static_cast<D3D11_BLEND_OP>( GetEnumStringMap( ).GetEnum( pBlendOpAlpha->GetValue( ), D3D11_BLEND_OP_ADD ) );
			}
			
			if ( const KeyValue* pDestBlend = pRenderTarget->Find( _T( "DestBlend" ) ) )
			{
				rtBlendDesc.DestBlend = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pDestBlend->GetValue( ), D3D11_BLEND_ZERO ) );
			}
			
			if ( const KeyValue* pDestBlendAlpha = pRenderTarget->Find( _T( "DestBlendAlpha" ) ) )
			{
				rtBlendDesc.DestBlendAlpha = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pDestBlendAlpha->GetValue( ), D3D11_BLEND_ZERO ) );
			}
			
			if ( const KeyValue* pSrcBlend = pRenderTarget->Find( _T( "SrcBlend" ) ) )
			{
				rtBlendDesc.SrcBlend = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pSrcBlend->GetValue( ), D3D11_BLEND_ONE ) );
			}
			
			if ( const KeyValue* pSrcBlendAlpha = pRenderTarget->Find( _T( "SrcBlendAlpha" ) ) )
			{
				rtBlendDesc.SrcBlendAlpha = static_cast<D3D11_BLEND>( GetEnumStringMap( ).GetEnum( pSrcBlendAlpha->GetValue( ), D3D11_BLEND_ONE ) );
			}
			
			if ( const KeyValue* pRenderTargetWriteMask = pRenderTarget->Find( _T( "RenderTargetWriteMask" ) ) )
			{
				rtBlendDesc.RenderTargetWriteMask = static_cast<UINT8>( GetEnumStringMap( ).GetEnum( pRenderTargetWriteMask->GetValue( ), D3D11_COLOR_WRITE_ENABLE_ALL ) );
			};
		}
		else if ( const KeyValue* pBlendFactor = desc->Find( _T( "BlendFactor" ) ) )
		{
			Stringstream ss( pBlendFactor->GetValue( ) );

			for ( auto factor = newDesc.m_blendFactor.begin( ); ss && factor != newDesc.m_blendFactor.end( ); ++factor )
			{
				ss >> *factor;
			}
		}
		else if ( const KeyValue* pSampleMask = desc->Find( _T( "SampleMask" ) ) )
		{
			Stringstream ss( pSampleMask->GetValue( ) );
			ss >> newDesc.m_sampleMask;
		}

		m_blendStateDesc.emplace( desc->GetValue( ), newDesc );
	}
}
