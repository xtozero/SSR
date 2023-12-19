#include "stdafx.h"
#include "CommonRenderResource.h"

#include "ColorTypes.h"

namespace
{
	agl::RefHandle<agl::Texture> CreateTexture( Color color, const char* debugName )
	{
		agl::TextureTrait trait = {
				.m_width = 1,
				.m_height = 1,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None
		};

		agl::ResourceInitData initData = {
			.m_srcData = &color.DWORD(),
			.m_srcSize = sizeof( color.DWORD() )
		};

		initData.m_sections.emplace_back();
		agl::ResourceSectionData& section = initData.m_sections.back();
		section.m_offset = 0;
		section.m_pitch = section.m_slicePitch = sizeof( uint32 );

		agl::RefHandle<agl::Texture> texture = agl::Texture::Create( trait, debugName, &initData );
		EnqueueRenderTask( [texture]()
			{
				texture->Init();
			} );

		return texture;
	}

	agl::RefHandle<agl::Texture> CreateCubeTexture( Color color, const char* debugName )
	{
		agl::TextureTrait trait = {
				.m_width = 1,
				.m_height = 1,
				.m_depth = 6,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::TextureCube
		};

		uint32 clearColor[6] = { color.DWORD(), color.DWORD(), color.DWORD(), color.DWORD(), color.DWORD(), color.DWORD() };
		agl::ResourceInitData initData = {
			.m_srcData = clearColor,
			.m_srcSize = sizeof( clearColor )
		};

		for ( int i = 0; i < 6; ++i )
		{
			initData.m_sections.emplace_back();
			agl::ResourceSectionData& section = initData.m_sections.back();

			section.m_offset = i * sizeof( uint32 );
			section.m_pitch = section.m_slicePitch = sizeof( uint32 );
		}

		agl::RefHandle<agl::Texture> cubeTexture = agl::Texture::Create( trait, debugName, &initData );
		EnqueueRenderTask( [cubeTexture]()
			{
				cubeTexture->Init();
			} );

		return cubeTexture;
	}
}

namespace rendercore
{
	REGISTER_GLOBAL_SHADER( FullScreenQuadVS, "./Assets/Shaders/Common/VS_FullScreenQuad.asset" );

	void DefaultGraphicsResources::BootUp()
	{
		BlackTexture = CreateTexture( Color::Black, "DefaultBlack" );
		WhiteTexture = CreateTexture( Color::White, "DefaultWhite" );

		BlackCubeTexture = CreateCubeTexture( Color::Black, "DefaultBlackCube" );
		WhiteCubeTexture = CreateCubeTexture( Color::White, "DefaultWhiteCube" );
	}

	void DefaultGraphicsResources::Shutdown()
	{
		EnqueueRenderTask( []()
			{
				BlackTexture = nullptr;
				WhiteTexture = nullptr;

				BlackCubeTexture = nullptr;
				WhiteCubeTexture = nullptr;
			} );
	}

	agl::RefHandle<agl::Texture> BlackTexture;
	agl::RefHandle<agl::Texture> WhiteTexture;

	agl::RefHandle<agl::Texture> BlackCubeTexture;
	agl::RefHandle<agl::Texture> WhiteCubeTexture;
}
