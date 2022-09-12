#include "JsonManufacturer.h"

#include "EnumStringMap.h"
#include "Json/json.hpp"
#include "ManufactureConfig.h"
#include "Material/Material.h"
#include "Math/Vector.h"
#include "RenderOption.h"
#include "SizedTypes.h"

namespace fs = std::filesystem;

namespace
{
	std::optional<uint32> ConvertAssetTypeToAssetID( const JSON::Value& root )
	{
		const JSON::Value* type = root.Find( "Type" );
		if ( type != nullptr )
		{
			const std::string& assetType = type->AsString();
			if ( assetType == "BlendOption" )
			{
				return rendercore::BlendOption::ID;
			}
			else if ( assetType == "DepthStencilOption" )
			{
				return rendercore::DepthStencilOption::ID;
			}
			else if ( assetType == "Material" )
			{
				return rendercore::Material::ID;
			}
			else if ( assetType == "RasterizerOption" )
			{
				return rendercore::RasterizerOption::ID;
			}
			else if ( assetType == "RenderOption" )
			{
				return rendercore::RenderOption::ID;
			}
			else if ( assetType == "SamplerOption" )
			{
				return rendercore::SamplerOption::ID;
			}
		}

		return {};
	}

	std::unique_ptr<AsyncLoadableAsset> CreateAssetByAssetID( uint32 assetID, const fs::path& assetPath, const JSON::Value& root )
	{
		std::unique_ptr<AsyncLoadableAsset> asset = nullptr;

		if ( assetID == rendercore::BlendOption::ID )
		{
			auto blendOption = std::make_unique<rendercore::BlendOption>();
			blendOption->SetPath( assetPath );

			if ( const JSON::Value* alphaToConverage = root.Find( "AlphaToConverage" ) )
			{
				blendOption->m_alphaToConverageEnable = alphaToConverage->AsBool();
			}

			if ( const JSON::Value* independentBlendEnable = root.Find( "IndependentBlendEnable" ) )
			{
				blendOption->m_independentBlendEnable = independentBlendEnable->AsBool();
			}

			if ( const JSON::Value* pRenderTargets = root.Find( "RenderTargets" ) )
			{
				const JSON::Value& renderTargets = *pRenderTargets;

				for ( size_t i = 0; i < renderTargets.Size(); ++i )
				{
					auto& rt = blendOption->m_renderTarget[i];
					if ( const JSON::Value* blendEnable = renderTargets[i].Find( "BlendEnable" ) )
					{
						rt.m_blendEnable = blendEnable->AsBool();
					}

					if ( const JSON::Value* srcBlend = renderTargets[i].Find( "SrcBlend" ) )
					{
						rt.m_srcBlend = GetEnum( srcBlend->AsString(), BLEND::ONE );
					}

					if ( const JSON::Value* destBlend = renderTargets[i].Find( "DestBlend" ) )
					{
						rt.m_destBlend = GetEnum( destBlend->AsString(), BLEND::ZERO );
					}

					if ( const JSON::Value* blendOp = renderTargets[i].Find( "BlendOp" ) )
					{
						rt.m_blendOp = GetEnum( blendOp->AsString(), BLEND_OP::ADD );
					}

					if ( const JSON::Value* srcBlendAlpha = renderTargets[i].Find( "SrcBlendAlpha" ) )
					{
						rt.m_srcBlendAlpha = GetEnum( srcBlendAlpha->AsString(), BLEND::ONE );
					}

					if ( const JSON::Value* destBlendAlpha = renderTargets[i].Find( "DestBlendAlpha" ) )
					{
						rt.m_destBlendAlpha = GetEnum( destBlendAlpha->AsString(), BLEND::ZERO );
					}

					if ( const JSON::Value* blendOpAlpha = renderTargets[i].Find( "BlendOpAlpha" ) )
					{
						rt.m_blendOpAlpha = GetEnum( blendOpAlpha->AsString(), BLEND_OP::ADD );
					}

					if ( const JSON::Value* renderTargetWriteMask = renderTargets[i].Find( "RenderTargetWriteMask" ) )
					{
						int32 writeMask = 0;

						for ( const JSON::Value& mask : *renderTargetWriteMask )
						{
							writeMask |= GetEnum( mask.AsString(), 0 );
						}

						rt.m_renderTargetWriteMask = static_cast<COLOR_WRITE_ENABLE>( writeMask );
					}
				}
			}

			if ( const JSON::Value* sampleMask = root.Find( "SampleMask" ) )
			{
				blendOption->m_sampleMask = static_cast<uint32>( sampleMask->AsInt() );
			}

			asset = std::move( blendOption );
		}
		else if ( assetID == rendercore::DepthStencilOption::ID )
		{
			auto depthStencilOption = std::make_unique<rendercore::DepthStencilOption>();
			depthStencilOption->SetPath( assetPath );

			auto& depthOption = depthStencilOption->m_depth;
			if ( const JSON::Value* depthEnable = root.Find( "DepthEnable" ) )
			{
				depthOption.m_enable = depthEnable->AsBool();
			}

			if ( const JSON::Value* depthWriteEnable = root.Find( "DepthWriteEnable" ) )
			{
				depthOption.m_writeDepth = depthWriteEnable->AsBool();
			}

			if ( const JSON::Value* depthFunc = root.Find( "DepthFunc" ) )
			{
				depthOption.m_depthFunc = GetEnum( depthFunc->AsString(), COMPARISON_FUNC::LESS );
			}

			auto stencilOption = depthStencilOption->m_stencil;
			if ( const JSON::Value* stencilEnable = root.Find( "StencilEnable" ) )
			{
				stencilOption.m_enable = stencilEnable->AsBool();
			}

			if ( const JSON::Value* stencilReadMask = root.Find( "StencilReadMask" ) )
			{
				stencilOption.m_readMask = static_cast<unsigned char>( stencilReadMask->AsInt() );
			}

			if ( const JSON::Value* stencilWriteMask = root.Find( "StencilWriteMask" ) )
			{
				stencilOption.m_writeMask = static_cast<unsigned char>( stencilWriteMask->AsInt() );
			}

			if ( const JSON::Value* stencilRef = root.Find( "StencilRef" ) )
			{
				stencilOption.m_ref = static_cast<uint32>( stencilRef->AsInt() );
			}

			auto& front = stencilOption.m_frontFace;
			if ( const JSON::Value* frontFaceStencilFunc = root.Find( "FrontFace.StencilFunc" ) )
			{
				front.m_func = GetEnum( frontFaceStencilFunc->AsString(), COMPARISON_FUNC::ALWAYS );
			}

			if ( const JSON::Value* frontFaceStencilDepthFailOp = root.Find( "FrontFace.StencilDepthFailOp" ) )
			{
				front.m_depthFailOp = GetEnum( frontFaceStencilDepthFailOp->AsString(), STENCIL_OP::KEEP );
			}

			if ( const JSON::Value* frontFaceStencilPassOp = root.Find( "FrontFace.StencilPassOp" ) )
			{
				front.m_passOp = GetEnum( frontFaceStencilPassOp->AsString(), STENCIL_OP::KEEP );
			}

			if ( const JSON::Value* frontFaceStencilFailOp = root.Find( "FrontFace.StencilFailOp" ) )
			{
				front.m_failOp = GetEnum( frontFaceStencilFailOp->AsString(), STENCIL_OP::KEEP );
			}

			auto& back = stencilOption.m_backFace;
			if ( const JSON::Value* backFaceStencilFunc = root.Find( "BackFace.StencilFunc" ) )
			{
				back.m_func = GetEnum( backFaceStencilFunc->AsString(), COMPARISON_FUNC::ALWAYS );
			}

			if ( const JSON::Value* backFaceStencilDepthFailOp = root.Find( "BackFace.StencilDepthFailOp" ) )
			{
				back.m_depthFailOp = GetEnum( backFaceStencilDepthFailOp->AsString(), STENCIL_OP::KEEP );
			}

			if ( const JSON::Value* backFaceStencilPassOp = root.Find( "BackFace.StencilPassOp" ) )
			{
				back.m_passOp = GetEnum( backFaceStencilPassOp->AsString(), STENCIL_OP::KEEP );
			}

			if ( const JSON::Value* backFaceStencilFailOp = root.Find( "BackFace.StencilFailOp" ) )
			{
				back.m_failOp = GetEnum( backFaceStencilFailOp->AsString(), STENCIL_OP::KEEP );
			}

			asset = std::move( depthStencilOption );
		}
		else if ( assetID == rendercore::Material::ID )
		{
			auto material = std::make_unique<rendercore::Material>();
			material->SetPath( assetPath );

			if ( const JSON::Value* shaderKeys = root.Find( "Shader" ) )
			{
				for ( uint32 i = 0; i < static_cast<uint32>( SHADER_TYPE::Count ); ++i )
				{
					auto shaderType = static_cast<SHADER_TYPE>( i );
					const JSON::Value* shaderPath = shaderKeys->Find( ToString( shaderType ) );
					if ( shaderPath == nullptr )
					{
						continue;
					}

					switch ( shaderType )
					{
					case SHADER_TYPE::VS:
					{
						auto vs = std::make_shared<rendercore::VertexShader>();
						vs->SetPath( shaderPath->AsString() );
						material->SetVertexShader( vs );
						break;
					}
					case SHADER_TYPE::HS:
						break;
					case SHADER_TYPE::DS:
						break;
					case SHADER_TYPE::GS:
					{
						auto gs = std::make_shared<rendercore::GeometryShader>();
						gs->SetPath( shaderPath->AsString() );
						material->SetGeometryShader( gs );
						break;
					}
					case SHADER_TYPE::PS:
					{
						auto ps = std::make_shared<rendercore::PixelShader>();
						ps->SetPath( shaderPath->AsString() );
						material->SetPixelShader( ps );
						break;
					}
					case SHADER_TYPE::CS:
						break;
					case SHADER_TYPE::Count:
						[[fallthrough]];
					case SHADER_TYPE::NONE:
						[[fallthrough]];
					default:
						break;
					}
				}
			}

			if ( const JSON::Value* surface = root.Find( "Surface" ) )
			{
				auto memberNames = surface->GetMemberNames();
				for ( auto name : memberNames )
				{
					if ( const JSON::Value* property = surface->Find( name ) )
					{
						switch ( property->Type() )
						{
						case JSON::DataType::STRING:
						{
							auto texture = std::make_shared<rendercore::Texture>();
							texture->SetPath( property->AsString() );

							material->AddProperty( name, texture );
							break;
						}
						case JSON::DataType::INTERGER:
						{
							material->AddProperty( name, property->AsInt() );
							break;
						}
						case JSON::DataType::REAL:
						{
							material->AddProperty( name, static_cast<float>( property->AsReal() ) );
							break;
						}
						case JSON::DataType::ARRAY:
						{
							Vector4 vector( 1.f, 1.f, 1.f, 1.f );

							for ( size_t i = 0; i < property->Size() && i < 4; ++i )
							{
								vector[i] = static_cast<float>( ( *property )[i].AsReal() );
							}

							material->AddProperty( name, vector );
							break;
						}
						case JSON::DataType::OBJECT:
							[[fallthrough]];
						case JSON::DataType::BOOLEAN:
							[[fallthrough]];
						case JSON::DataType::EMPTY:
							[[fallthrough]];
						default:
							break;
						}
					}
				}
			}

			if ( const JSON::Value* sampler = root.Find( "Sampler" ) )
			{
				auto memberNames = sampler->GetMemberNames();
				for ( auto name : memberNames )
				{
					if ( const JSON::Value* path = sampler->Find( name ) )
					{
						auto samplerOption = std::make_shared<rendercore::SamplerOption>();
						samplerOption->SetPath( path->AsString() );

						material->AddSampler( name, samplerOption );
					}
				}
			}

			asset = std::move( material );
		}
		else if ( assetID == rendercore::RasterizerOption::ID )
		{
			auto rasterizerOption = std::make_unique<rendercore::RasterizerOption>();
			rasterizerOption->SetPath( assetPath );

			if ( const JSON::Value* wireframe = root.Find( "Wireframe" ) )
			{
				rasterizerOption->m_isWireframe = wireframe->AsBool();
			}

			if ( const JSON::Value* cullMode = root.Find( "CullMode" ) )
			{
				rasterizerOption->m_cullMode = GetEnum( cullMode->AsString(), CULL_MODE::BACK );
			}

			if ( const JSON::Value* counterClockwise = root.Find( "CounterClockwise" ) )
			{
				rasterizerOption->m_counterClockwise = counterClockwise->AsBool();
			}

			if ( const JSON::Value* depthBias = root.Find( "DepthBias" ) )
			{
				rasterizerOption->m_depthBias = depthBias->AsInt();
			}

			if ( const JSON::Value* depthClipEnable = root.Find( "DepthClipEnable" ) )
			{
				rasterizerOption->m_depthClipEnable = depthClipEnable->AsBool();
			}

			if ( const JSON::Value* scissorEnable = root.Find( "ScissorEnable" ) )
			{
				rasterizerOption->m_scissorEnable = scissorEnable->AsBool();
			}

			if ( const JSON::Value* multisampleEnalbe = root.Find( "MultisampleEnable" ) )
			{
				rasterizerOption->m_multisampleEnalbe = multisampleEnalbe->AsBool();
			}

			if ( const JSON::Value* antialiasedLineEnable = root.Find( "AntialiasedLineEnable" ) )
			{
				rasterizerOption->m_antialiasedLineEnable = antialiasedLineEnable->AsInt();
			}

			asset = std::move( rasterizerOption );
		}
		else if ( assetID == rendercore::SamplerOption::ID )
		{
			auto samplerOption = std::make_unique<rendercore::SamplerOption>();
			samplerOption->SetPath( assetPath );

			if ( const JSON::Value* filter = root.Find( "Filter" ) )
			{
				samplerOption->m_filter = 0;
				for ( auto option : *filter )
				{
					samplerOption->m_filter |= GetEnum( option.AsString(), 0 );
				}
			}

			if ( const JSON::Value* addressU = root.Find( "AddressU" ) )
			{
				samplerOption->m_addressU = GetEnum( addressU->AsString(), TEXTURE_ADDRESS_MODE::CLAMP );
			}

			if ( const JSON::Value* addressV = root.Find( "AddressV" ) )
			{
				samplerOption->m_addressV = GetEnum( addressV->AsString(), TEXTURE_ADDRESS_MODE::CLAMP );
			}

			if ( const JSON::Value* addressW = root.Find( "AddressW" ) )
			{
				samplerOption->m_addressW = GetEnum( addressW->AsString(), TEXTURE_ADDRESS_MODE::CLAMP );
			}

			if ( const JSON::Value* mipLODBias = root.Find( "MipLODBias" ) )
			{
				samplerOption->m_mipLODBias = static_cast<float>( mipLODBias->AsReal() );
			}

			if ( const JSON::Value* comparisonFunc = root.Find( "ComparisonFunc" ) )
			{
				samplerOption->m_comparisonFunc = GetEnum( comparisonFunc->AsString(), COMPARISON_FUNC::NEVER );
			}

			asset = std::move( samplerOption );
		}
		else if ( assetID == rendercore::RenderOption::ID )
		{
			auto renderOption = std::make_unique<rendercore::RenderOption>();

			if ( const JSON::Value* blendOptionPath = root.Find( "Blend" ) )
			{
				auto blendOption = std::make_shared<rendercore::BlendOption>();
				blendOption->SetPath( fs::path( blendOptionPath->AsString() ) );
				renderOption->m_blendOption = blendOption;
			}

			if ( const JSON::Value* depthStencilOptionPath = root.Find( "DS_State" ) )
			{
				auto depthStencilOption = std::make_shared<rendercore::DepthStencilOption>();
				depthStencilOption->SetPath( fs::path( depthStencilOptionPath->AsString() ) );
				renderOption->m_depthStencilOption = depthStencilOption;
			}

			if ( const JSON::Value* rasterizerOptionPath = root.Find( "RS_State" ) )
			{
				auto rasterizerOption = std::make_shared<rendercore::RasterizerOption>();
				rasterizerOption->SetPath( fs::path( rasterizerOptionPath->AsString() ) );
				renderOption->m_rasterizerOption = rasterizerOption;
			}

			asset = std::move( renderOption );
		}

		return asset;
	}

	bool ValidateJsonAsset( const AsyncLoadableAsset* asset, const Archive& ar )
	{
		Archive rAr( ar.Data(), ar.Size() );
		uint32 assetID = 0;
		rAr << assetID;

		if ( assetID == rendercore::BlendOption::ID )
		{
			rendercore::BlendOption blendOption;
			blendOption.Serialize( rAr );

			return ( blendOption == ( *reinterpret_cast<const rendercore::BlendOption*>( asset ) ) );
		}
		else if ( assetID == rendercore::DepthStencilOption::ID )
		{
			rendercore::DepthStencilOption depthStencilOption;
			depthStencilOption.Serialize( rAr );

			return ( depthStencilOption == ( *reinterpret_cast<const rendercore::DepthStencilOption*>( asset ) ) );
		}
		else if ( assetID == rendercore::RasterizerOption::ID )
		{
			rendercore::RasterizerOption rasterizerOption;
			rasterizerOption.Serialize( rAr );

			return ( rasterizerOption == ( *reinterpret_cast<const rendercore::RasterizerOption*>( asset ) ) );
		}
		else if ( assetID == rendercore::SamplerOption::ID )
		{
			rendercore::SamplerOption samplerOption;
			samplerOption.Serialize( rAr );

			return ( samplerOption == ( *reinterpret_cast<const rendercore::SamplerOption*>( asset ) ) );
		}
		else if ( assetID == rendercore::RenderOption::ID )
		{
			rendercore::RenderOption renderOption;
			renderOption.Serialize( rAr );

			return ( renderOption == ( *reinterpret_cast<const rendercore::RenderOption*>( asset ) ) );
		}

		return false;
	}
}

bool JsonManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension() == fs::path( ".json" );
}

std::optional<Products> JsonManufacturer::Manufacture( const PathEnvironment& env, const std::filesystem::path& path ) const
{
	if ( fs::exists( path ) == false )
	{
		return { };
	}

	JSON::Value root;
	JSON::Reader reader;
	if ( reader.Parse( path, root ) == false )
	{
		return { };
	}

	auto assetID = ConvertAssetTypeToAssetID( root );
	if ( !assetID )
	{
		return { };
	}

	fs::path assetPath = "." / fs::relative( env.m_destination, env.m_destination.parent_path() );
	assetPath /= fs::relative( path, env.m_source );
	assetPath.replace_extension( ".Asset" );
	std::unique_ptr<AsyncLoadableAsset> asset = CreateAssetByAssetID( assetID.value(), assetPath, root );
	if ( asset == nullptr )
	{
		return { };
	}

	asset->SetLastWriteTime( fs::last_write_time( path ) );

	Archive ar;
	asset->Serialize( ar );

#ifdef ASSET_VALIDATE
	if ( ValidateJsonAsset( asset.get(), ar ) == false )
	{
		DebugBreak();
	}
#endif

	Products products;
	products.emplace_back( path.filename(), std::move( ar ) );
	return products;
}
