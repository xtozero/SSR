#pragma once

#include "Texture.h"

namespace rendercore
{
	class DepthStencil
	{
	public:
		void UpdateBufferSize( UINT width, UINT height );
		void Clear( float depth, UINT8 stencil );

		aga::Texture* Texture( ) { return m_depthStencil.Get( ); }

	private:
		RefHandle<aga::Texture> m_depthStencil;
	};
}
