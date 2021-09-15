#pragma once

#include "SizedTypes.h"
#include "Texture.h"

namespace rendercore
{
	class DepthStencil
	{
	public:
		void UpdateBufferSize( uint32 width, uint32 height );
		void Clear( float depth, uint8 stencil );

		aga::Texture* Texture( ) { return m_depthStencil.Get( ); }

	private:
		RefHandle<aga::Texture> m_depthStencil;
	};
}
