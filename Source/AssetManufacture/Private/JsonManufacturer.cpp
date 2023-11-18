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
	std::optional<uint32> ConvertAssetTypeToAssetID( const json::Value& root )
	{
		const json::Value* type = root.Find( "Type" );
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

	rendercore::SamplerOption CreateSamplerOption( const json::Value& desc )
	{
		rendercore::SamplerOption samplerOption;

		if ( const json::Value* filter = desc.Find( "Filter" ) )
		{
			samplerOption.m_filter = agl::TextureFilter::Point;
			for ( auto option : *filter )
			{
				samplerOption.m_filter |= GetEnum( option.AsString(), agl::TextureFilter::Point );
			}
		}

		if ( const json::Value* addressU = desc.Find( "AddressU" ) )
		{
			samplerOption.m_addressU = GetEnum( addressU->AsString(), agl::TextureAddressMode::Clamp );
		}

		if ( const json::Value* addressV = desc.Find( "AddressV" ) )
		{
			samplerOption.m_addressV = GetEnum( addressV->AsString(), agl::TextureAddressMode::Clamp );
		}

		if ( const json::Value* addressW = desc.Find( "AddressW" ) )
		{
			samplerOption.m_addressW = GetEnum( addressW->AsString(), agl::TextureAddressMode::Clamp );
		}

		if ( const json::Value* mipLODBias = desc.Find( "MipLODBias" ) )
		{
			samplerOption.m_mipLODBias = static_cast<float>( mipLODBias->AsReal() );
		}

		if ( const json::Value* comparisonFunc = desc.Find( "ComparisonFunc" ) )
		{
			samplerOption.m_comparisonFunc = GetEnum( comparisonFunc->AsString(), agl::ComparisonFunc::Never );
		}

		return samplerOption;
	}

	std::unique_ptr<AsyncLoadableAsset> CreateAssetByAssetID( uint32 assetID, const fs::path& assetPath, const json::Value& root )
	{
		std::unique_ptr<AsyncLoadableAsset> asset = nullptr;

		if ( assetID == rendercore::BlendOption::ID )
		{
			auto blendOption = std::make_unique<rendercore::BlendOption>();
			blendOption->SetPath( assetPath );

			if ( const json::Value* alphaToConverage = root.Find( "AlphaToConverage" ) )
			{
				blendOption->m_alphaToConverageEnable = alphaToConverage->AsBool();
			}

			if ( const json::Value* independentBlendEnable = root.Find( "IndependentBlendEnable" ) )
			{
				blendOption->m_independentBlendEnable = independentBlendEnable->AsBool();
			}

			if ( const json::Value* pRenderTargets = root.Find( "RenderTargets" ) )
			{
				const json::Value& renderTargets = *pRenderTargets;

				for ( size_t i = 0; i < renderTargets.Size(); ++i )
				{
					auto& rt = blendOption->m_renderTarget[i];
					if ( const json::Value* blendEnable = renderTargets[i].Find( "BlendEnable" ) )
					{
						rt.m_blendEnable = blendEnable->AsBool();
					}

					if ( const json::Value* srcBlend = renderTargets[i].Find( "SrcBlend" ) )
					{
						rt.m_srcBlend = GetEnum( srcBlend->AsString(), agl::Blend::One );
					}

					if ( const json::Value* destBlend = renderTargets[i].Find( "DestBlend" ) )
					{
						rt.m_destBlend = GetEnum( destBlend->AsString(), agl::Blend::Zero );
					}

					if ( const json::Value* blendOp = renderTargets[i].Find( "BlendOp" ) )
					{
						rt.m_blendOp = GetEnum( blendOp->AsString(), agl::BlendOp::Add );
					}

					if ( const json::Value* srcBlendAlpha = renderTargets[i].Find( "SrcBlendAlpha" ) )
					{
						rt.m_srcBlendAlpha = GetEnum( srcBlendAlpha->AsString(), agl::Blend::One );
					}

					if ( const json::Value* destBlendAlpha = renderTargets[i].Find( "DestBlendAlpha" ) )
					{
						rt.m_destBlendAlpha = GetEnum( destBlendAlpha->AsString(), agl::Blend::Zero );
					}

					if ( const json::Value* blendOpAlpha = renderTargets[i].Find( "BlendOpAlpha" ) )
					{
						rt.m_blendOpAlpha = GetEnum( blendOpAlpha->AsString(), agl::BlendOp::Add );
					}

					if ( const json::Value* renderTargetWriteMask = renderTargets[i].Find( "RenderTargetWriteMask" ) )
					{
						int32 writeMask = 0;

						for ( const json::Value& mask : *renderTargetWriteMask )
						{
							writeMask |= GetEnum( mask.AsString(), 0 );
						}

						rt.m_renderTargetWriteMask = static_cast<agl::ColorWriteEnable>( writeMask );
					}
				}
			}

			if ( const json::Value* sampleMask = root.Find( "SampleMask" ) )
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
			if ( const json::Value* depthEnable = root.Find( "DepthEnable" ) )
			{
				depthOption.m_enable = depthEnable->AsBool();
			}

			if ( const json::Value* depthWriteEnable = root.Find( "DepthWriteEnable" ) )
			{
				depthOption.m_writeDepth = depthWriteEnable->AsBool();
			}

			if ( const json::Value* depthFunc = root.Find( "DepthFunc" ) )
			{
				depthOption.m_depthFunc = GetEnum( depthFunc->AsString(), agl::ComparisonFunc::Less );
			}

			auto stencilOption = depthStencilOption->m_stencil;
			if ( const json::Value* stencilEnable = root.Find( "StencilEnable" ) )
			{
				stencilOption.m_enable = stencilEnable->AsBool();
			}

			if ( const json::Value* stencilReadMask = root.Find( "StencilReadMask" ) )
			{
				stencilOption.m_readMask = static_cast<unsigned char>( stencilReadMask->AsInt() );
			}

			if ( const json::Value* stencilWriteMask = root.Find( "StencilWriteMask" ) )
			{
				stencilOption.m_writeMask = static_cast<unsigned char>( stencilWriteMask->AsInt() );
			}

			if ( const json::Value* stencilRef = root.Find( "StencilRef" ) )
			{
				stencilOption.m_ref = static_cast<uint32>( stencilRef->AsInt() );
			}

			auto& front = stencilOption.m_frontFace;
			if ( const json::Value* frontFaceStencilFunc = root.Find( "FrontFace.StencilFunc" ) )
			{
				front.m_func = GetEnum( frontFaceStencilFunc->AsString(), agl::ComparisonFunc::Always );
			}

			if ( const json::Value* frontFaceStencilDepthFailOp = root.Find( "FrontFace.StencilDepthFailOp" ) )
			{
				front.m_depthFailOp = GetEnum( frontFaceStencilDepthFailOp->AsString(), agl::StencilOp::Keep );
			}

			if ( const json::Value* frontFaceStencilPassOp = root.Find( "FrontFace.StencilPassOp" ) )
			{
				front.m_passOp = GetEnum( frontFaceStencilPassOp->AsString(), agl::StencilOp::Keep );
			}

			if ( const json::Value* frontFaceStencilFailOp = root.Find( "FrontFace.StencilFailOp" ) )
			{
				front.m_failOp = GetEnum( frontFaceStencilFailOp->AsString(), agl::StencilOp::Keep );
			}

			auto& back = stencilOption.m_backFace;
			if ( const json::Value* backFaceStencilFunc = root.Find( "BackFace.StencilFunc" ) )
			{
				back.m_func = GetEnum( backFaceStencilFunc->AsString(), agl::ComparisonFunc::Always );
			}

			if ( const json::Value* backFaceStencilDepthFailOp = root.Find( "BackFace.StencilDepthFailOp" ) )
			{
				back.m_depthFailOp = GetEnum( backFaceStencilDepthFailOp->AsString(), agl::StencilOp::Keep );
			}

			if ( const json::Value* backFaceStencilPassOp = root.Find( "BackFace.StencilPassOp" ) )
			{
				back.m_passOp = GetEnum( backFaceStencilPassOp->AsString(), agl::StencilOp::Keep );
			}

			if ( const json::Value* backFaceStencilFailOp = root.Find( "BackFace.StencilFailOp" ) )
			{
				back.m_failOp = GetEnum( backFaceStencilFailOp->AsString(), agl::StencilOp::Keep );
			}

			asset = std::move( depthStencilOption );
		}
		else if ( assetID == rendercore::Material::ID )
		{
			auto material = std::make_unique<rendercore::Material>();
			material->SetPath( assetPath );

			if ( const json::Value* shaderKeys = root.Find( "Shader" ) )
			{
				for ( uint32 i = 0; i < static_cast<uint32>( agl::ShaderType::Count ); ++i )
				{
					auto shaderType = static_cast<agl::ShaderType>( i );
					const json::Value* shaderPath = shaderKeys->Find( ToString( shaderType ) );
					if ( shaderPath == nullptr )
					{
						continue;
					}

					switch ( shaderType )
					{
					case agl::ShaderType::VS:
					{
						auto vs = std::make_shared<rendercore::VertexShader>();
						vs->SetPath( shaderPath->AsString() );
						material->SetVertexShader( vs );
						break;
					}
					case agl::ShaderType::HS:
						break;
					case agl::ShaderType::DS:
						break;
					case agl::ShaderType::GS:
					{
						auto gs = std::make_shared<rendercore::GeometryShader>();
						gs->SetPath( shaderPath->AsString() );
						material->SetGeometryShader( gs );
						break;
					}
					case agl::ShaderType::PS:
					{
						auto ps = std::make_shared<rendercore::PixelShader>();
						ps->SetPath( shaderPath->AsString() );
						material->SetPixelShader( ps );
						break;
					}
					case agl::ShaderType::CS:
						break;
					case agl::ShaderType::Count:
						[[fallthrough]];
					case agl::ShaderType::None:
						[[fallthrough]];
					default:
						break;
					}
				}
			}

			if ( const json::Value* surface = root.Find( "Surface" ) )
			{
				auto memberNames = surface->GetMemberNames();
				for ( auto name : memberNames )
				{
					if ( const json::Value* property = surface->Find( name ) )
					{
						switch ( property->Type() )
						{
						case json::DataType::String:
						{
							auto texture = std::make_shared<rendercore::Texture>();
							texture->SetPath( property->AsString() );

							material->AddProperty( name, texture );
							break;
						}
						case json::DataType::Interger:
						{
							material->AddProperty( name, property->AsInt() );
							break;
						}
						case json::DataType::Real:
						{
							material->AddProperty( name, static_cast<float>( property->AsReal() ) );
							break;
						}
						case json::DataType::Array:
						{
							Vector4 vector( 1.f, 1.f, 1.f, 1.f );

							for ( size_t i = 0; i < property->Size() && i < 4; ++i )
							{
								vector[i] = static_cast<float>( ( *property )[i].AsReal() );
							}

							material->AddProperty( name, vector );
							break;
						}
						case json::DataType::Object:
							[[fallthrough]];
						case json::DataType::Boolean:
							[[fallthrough]];
						case json::DataType::Empty:
							[[fallthrough]];
						default:
							break;
						}
					}
				}
			}

			if ( const json::Value* sampler = root.Find( "Sampler" ) )
			{
				auto memberNames = sampler->GetMemberNames();
				for ( auto name : memberNames )
				{
					if ( const json::Value* desc = sampler->Find( name ) )
					{
						rendercore::SamplerOption samplerOption = CreateSamplerOption( *desc );

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

			if ( const json::Value* wireframe = root.Find( "Wireframe" ) )
			{
				rasterizerOption->m_isWireframe = wireframe->AsBool();
			}

			if ( const json::Value* cullMode = root.Find( "CullMode" ) )
			{
				rasterizerOption->m_cullMode = GetEnum( cullMode->AsString(), agl::CullMode::Back );
			}

			if ( const json::Value* counterClockwise = root.Find( "CounterClockwise" ) )
			{
				rasterizerOption->m_counterClockwise = counterClockwise->AsBool();
			}

			if ( const json::Value* depthBias = root.Find( "DepthBias" ) )
			{
				rasterizerOption->m_depthBias = depthBias->AsInt();
			}

			if ( const json::Value* depthClipEnable = root.Find( "DepthClipEnable" ) )
			{
				rasterizerOption->m_depthClipEnable = depthClipEnable->AsBool();
			}

			if ( const json::Value* scissorEnable = root.Find( "ScissorEnable" ) )
			{
				rasterizerOption->m_scissorEnable = scissorEnable->AsBool();
			}

			if ( const json::Value* multisampleEnable = root.Find( "MultisampleEnable" ) )
			{
				rasterizerOption->m_multisampleEnable = multisampleEnable->AsBool();
			}

			if ( const json::Value* antialiasedLineEnable = root.Find( "AntialiasedLineEnable" ) )
			{
				rasterizerOption->m_antialiasedLineEnable = antialiasedLineEnable->AsInt();
			}

			asset = std::move( rasterizerOption );
		}
		else if ( assetID == rendercore::SamplerOption::ID )
		{
			auto samplerOption = std::make_unique<rendercore::SamplerOption>();
			samplerOption->SetPath( assetPath );

			if ( const json::Value* filter = root.Find( "Filter" ) )
			{
				samplerOption->m_filter = agl::TextureFilter::Point;
				for ( auto option : *filter )
				{
					samplerOption->m_filter |= GetEnum( option.AsString(), agl::TextureFilter::Point );
				}
			}

			if ( const json::Value* addressU = root.Find( "AddressU" ) )
			{
				samplerOption->m_addressU = GetEnum( addressU->AsString(), agl::TextureAddressMode::Clamp );
			}

			if ( const json::Value* addressV = root.Find( "AddressV" ) )
			{
				samplerOption->m_addressV = GetEnum( addressV->AsString(), agl::TextureAddressMode::Clamp );
			}

			if ( const json::Value* addressW = root.Find( "AddressW" ) )
			{
				samplerOption->m_addressW = GetEnum( addressW->AsString(), agl::TextureAddressMode::Clamp );
			}

			if ( const json::Value* mipLODBias = root.Find( "MipLODBias" ) )
			{
				samplerOption->m_mipLODBias = static_cast<float>( mipLODBias->AsReal() );
			}

			if ( const json::Value* comparisonFunc = root.Find( "ComparisonFunc" ) )
			{
				samplerOption->m_comparisonFunc = GetEnum( comparisonFunc->AsString(), agl::ComparisonFunc::Never );
			}

			asset = std::move( samplerOption );
		}
		else if ( assetID == rendercore::RenderOption::ID )
		{
			auto renderOption = std::make_unique<rendercore::RenderOption>();

			if ( const json::Value* blendOptionPath = root.Find( "Blend" ) )
			{
				auto blendOption = std::make_shared<rendercore::BlendOption>();
				blendOption->SetPath( fs::path( blendOptionPath->AsString() ) );
				renderOption->m_blendOption = blendOption;
			}

			if ( const json::Value* depthStencilOptionPath = root.Find( "DS_State" ) )
			{
				auto depthStencilOption = std::make_shared<rendercore::DepthStencilOption>();
				depthStencilOption->SetPath( fs::path( depthStencilOptionPath->AsString() ) );
				renderOption->m_depthStencilOption = depthStencilOption;
			}

			if ( const json::Value* rasterizerOptionPath = root.Find( "RS_State" ) )
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
	fs::path extension = ToLower( srcPath.extension().generic_string() );
	return extension == fs::path( ".json" );
}

std::optional<Products> JsonManufacturer::Manufacture( const PathEnvironment& env, const std::filesystem::path& path ) const
{
	if ( fs::exists( path ) == false )
	{
		return { };
	}

	json::Value root;
	json::Reader reader;
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
