#include "stdafx.h"
#include "GraphicsResource/RenderOptionManager.h"

#include "Core/InterfaceFactories.h"
#include "FileSystem/EngineFileSystem.h"
#include "GraphicsResource/RenderOption.h"
#include "Json/json.hpp"

#include <memory>
#include <unordered_map>

class RenderOptionManager : public IRenderOptionManager
{
public:
	virtual bool Bootup( ) override;
	virtual const RenderOption* GetRenderOption( const std::string& name ) const override;

	RenderOptionManager( ) = default;
	RenderOptionManager( const RenderOptionManager& ) = delete;
	RenderOptionManager( RenderOptionManager&& ) = delete;
	RenderOptionManager& operator=( const RenderOptionManager& ) = delete;
	RenderOptionManager& operator=( RenderOptionManager&& ) = delete;

private:
	enum RenderOptionAssetType
	{
		RenderOptionAsset = 0,
		BlendOptionAsset,
		DepthStencilOptionAsset,
		RasterizerOptionAsset,
		SamplerOptionAsset,
		MaxOptionAssetType
	};

	struct RenderOptionLoadContext
	{
		char* m_asset[MaxOptionAssetType] = { };
		unsigned long m_assetSize[MaxOptionAssetType] = { };
		bool m_readSuccess[MaxOptionAssetType] = { };

		~RenderOptionLoadContext( )
		{
			for ( int assetType = RenderOptionAsset; assetType < MaxOptionAssetType; ++assetType )
			{
				delete[] m_asset[assetType];
				m_asset[assetType] = nullptr;
			}
		}
	};

	bool LoadRenderOptionAsset( const char* assetPath, std::shared_ptr<RenderOptionLoadContext>& context, int assetType );

	void OnSuccessFileRead( const std::shared_ptr<RenderOptionLoadContext>& context );
	void ParseRenderOption( const std::shared_ptr<RenderOptionLoadContext>& context );

	std::unordered_map<std::string, RenderOption> m_renderOptions;
	std::atomic<bool> m_isBootUpProgress;
};

bool RenderOptionManager::Bootup( )
{
	std::shared_ptr<RenderOptionLoadContext> context = std::make_shared<RenderOptionLoadContext>( );

	const char* assetPaths[MaxOptionAssetType] = {
		"./Scripts/RenderOption/RenderOption.json",
		"./Scripts/RenderOption/BlendOption.json",
		"./Scripts/RenderOption/DepthStencilOption.json",
		"./Scripts/RenderOption/RasterizerOption.json",
		"./Scripts/RenderOption/SamplerOption.json"
	};

	for ( int assetType = RenderOptionAsset; assetType < MaxOptionAssetType; ++assetType )
	{
		if ( LoadRenderOptionAsset( assetPaths[assetType], context, assetType ) == false )
		{
			return false;
		}
	}

	m_isBootUpProgress = true;
	return true;
}

const RenderOption* RenderOptionManager::GetRenderOption( const std::string& name ) const
{
	if ( m_isBootUpProgress )
	{
		__debugbreak( );
	}
	else
	{
		auto found = m_renderOptions.find( name );
		assert( found != m_renderOptions.end( ) );
		return &found->second;
	}
}

bool RenderOptionManager::LoadRenderOptionAsset( const char* assetPath, std::shared_ptr<RenderOptionLoadContext>& context, int assetType )
{
	IFileSystem* fileSystem = GetInterface<IFileSystem>( );
	FileHandle renderOptionAsset = fileSystem->OpenFile( assetPath );
	
	if ( renderOptionAsset.IsValid( ) == false )
	{
		return false;
	}

	unsigned long fileSize = fileSystem->GetFileSize( renderOptionAsset );
	context->m_assetSize[assetType] = fileSize;
	context->m_asset[assetType] = new char[fileSize];

	IFileSystem::IOCompletionCallback ParseRenderOptionAsset;
	ParseRenderOptionAsset.BindFunctor(
		[this, context, assetType, renderOptionAsset]( const char* /*buffer*/, unsigned long /*bufferSize*/ )
		{
			context->m_readSuccess[assetType] = true;
			OnSuccessFileRead( context );
			GetInterface<IFileSystem>( )->CloseFile( renderOptionAsset );
		}
	);

	bool result = fileSystem->ReadAsync( renderOptionAsset, context->m_asset[assetType], fileSize, &ParseRenderOptionAsset );
	if ( result == false )
	{
		GetInterface<IFileSystem>( )->CloseFile( renderOptionAsset );
	}

	return result;
}

void RenderOptionManager::OnSuccessFileRead( const std::shared_ptr<RenderOptionLoadContext>& context )
{
	bool doParsing = std::all_of( std::begin( context->m_readSuccess ), std::end( context->m_readSuccess ), []( bool readSuccess ) { return readSuccess; } );

	if ( doParsing )
	{
		ParseRenderOption( context );
	}
}

void RenderOptionManager::ParseRenderOption( const std::shared_ptr<RenderOptionLoadContext>& context )
{
	JSON::Value renderOption[MaxOptionAssetType];

	for ( int assetType = RenderOptionAsset; assetType < MaxOptionAssetType; ++assetType )
	{
		const char* asset = context->m_asset[assetType];
		unsigned long assetSize = context->m_assetSize[assetType];

		JSON::Reader reader;
		if ( reader.Parse( asset, assetSize, renderOption[assetType] ) == false )
		{
			__debugbreak( );
		}
	}

	for ( const JSON::Value& option : renderOption[RenderOptionAsset] )
	{
		std::vector<const char*> name = option.GetMemberNames();
		if ( name.size( ) != 1 )
		{
			continue;
		}

		auto result = m_renderOptions.emplace( name[0], RenderOption( ) );
		RenderOption& newRenderOption = result.first->second;

		const JSON::Value& optionScript = *option.Find( name[0] );
		newRenderOption.LoadFromScript( optionScript, 
										renderOption[BlendOptionAsset],
										renderOption[DepthStencilOptionAsset],
										renderOption[RasterizerOptionAsset],
										renderOption[SamplerOptionAsset]);
	}

	m_isBootUpProgress = false;
}

Owner<IRenderOptionManager*> CreateRenderOptionManager( )
{
	IRenderOptionManager* pRenderOptionManager = new RenderOptionManager;
	pRenderOptionManager->Bootup( );
	return pRenderOptionManager;
}

void DestoryRenderOptionManager( Owner<IRenderOptionManager*> pRenderOptionManager )
{
	delete pRenderOptionManager;
}
