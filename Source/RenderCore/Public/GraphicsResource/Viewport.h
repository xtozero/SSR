#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class ICommandListBase;
	class Texture;
	class Viewport;
}

namespace rendercore
{
	class Canvas;

	class HitProxyMap
	{
	public:
		void Init( uint32 width, uint32 height );

		agl::Texture* Texture() const;
		agl::Texture* CpuTexture() const;

	private:
		uint32 m_width = 0;
		uint32 m_height = 0;

		RefHandle<agl::Texture> m_hitProxyTexture;
		RefHandle<agl::Texture> m_hitProxyCpuTexture;
	};

	class Viewport final
	{
	public:
		RENDERCORE_DLL void Clear();
		RENDERCORE_DLL void Bind( agl::ICommandListBase& commandList ) const;

		RENDERCORE_DLL std::pair<uint32, uint32> Size() const;
		RENDERCORE_DLL std::pair<uint32, uint32> SizeOnRenderThread() const;

		RENDERCORE_DLL void Resize( const std::pair<uint32, uint32>& newSize );

		RENDERCORE_DLL agl::Texture* Texture();

		HitProxyMap& GetHitPorxyMap();

		RENDERCORE_DLL Viewport( uint32 width, uint32 height, agl::ResourceFormat format, const float4& bgColor );
		RENDERCORE_DLL explicit Viewport( rendercore::Canvas& canvas );
		RENDERCORE_DLL ~Viewport();

	private:
		RefHandle<agl::Viewport> m_pViewport;
		HitProxyMap m_hitProxyMap;
		ColorF m_clearColor;
	};
}