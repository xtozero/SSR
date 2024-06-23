#include "EngineDefaultManufacturer.h"

#include "CommandList.h"
#include "DDSTexture.h"
#include "DirectXTex.h"
#include "DirectXTexTool.h"
#include "ManufactureConfig.h"
#include "RenderUtility/CommonRenderResource.h"
#include "TaskScheduler.h"

#include <chrono>

namespace fs = std::filesystem;

void EngineDefaultManufacturer::Manufacture( std::set<std::filesystem::path>& outProcessed )
{
	CreateBRDFLookUpTexture( outProcessed );
}

void EngineDefaultManufacturer::CreateBRDFLookUpTexture( std::set<std::filesystem::path>& outProcessed )
{
	fs::path target = ManufactureConfig::Instance().RootDirectory() / "Assets/EngineDefault/Texture" / "PrecomputedBRDF.asset";
	target = fs::absolute( target.make_preferred() );

	outProcessed.emplace( target );

	if ( fs::exists( target ) )
	{
		return;
	}

	fs::path targetDirectory = target.parent_path();
	if ( fs::exists( targetDirectory ) == false )
	{
		fs::create_directories( targetDirectory );
	}

	DirectX::ScratchImage image;

	TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>( 
		[&image]()
		{
			RefHandle<agl::Texture> brdfLUT = rendercore::CreateBRDFLookUpTexture();
			GetInterface<agl::IAgl>()->WaitGPU();

			auto commandList = rendercore::GetCommandList();
			
			bool result = commandList.CaptureTexture( brdfLUT, image );
			assert( result && "CaptureTexture was Failed" );
		} 
	);
	GetInterface<ITaskScheduler>()->Wait( handle );

	rendercore::DDSTextureInitializer initializer = ConvertToBCTextureInitializer( image );

	rendercore::DDSTexture asset( initializer );

	asset.SetLastWriteTime( std::chrono::file_clock::now() );

	Archive ar;
	asset.Serialize( ar );

	ar.WriteToFile( target );
}
