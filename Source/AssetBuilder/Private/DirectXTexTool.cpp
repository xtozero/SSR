#include "DirectXTexTool.h"

#include "DirectXTex.h"
#include "../DXGI/DxgiFlagConvertor.h"

uint32 ConvertToBCTextureDemension( DirectX::TEX_DIMENSION dimension )
{
	switch ( dimension )
	{
	case DirectX::TEX_DIMENSION_TEXTURE1D:
		return 1;
		break;
	case DirectX::TEX_DIMENSION_TEXTURE2D:
		return 2;
		break;
	case DirectX::TEX_DIMENSION_TEXTURE3D:
		return 3;
		break;
	default:
		break;
	}

	return 0;
}

rendercore::DDSTextureInitializer ConvertToBCTextureInitializer( const DirectX::ScratchImage& image )
{
	rendercore::DDSTextureInitializer initializer;

	const DirectX::TexMetadata& meta = image.GetMetadata();
	initializer.m_width = static_cast<uint32>( meta.width );
	initializer.m_height = static_cast<uint32>( meta.height );
	initializer.m_depth = static_cast<uint32>( meta.depth );
	initializer.m_arraySize = static_cast<uint32>( meta.arraySize );
	initializer.m_mipLevels = static_cast<uint32>( meta.mipLevels );

	initializer.m_isCubeMap = meta.IsCubemap();
	initializer.m_demension = ConvertToBCTextureDemension( meta.dimension );

	initializer.m_format = agl::ConvertDxgiFormatToFormat( meta.format );

	initializer.m_size = static_cast<uint32>( image.GetPixelsSize() );
	initializer.m_memory = image.GetPixels();

	for ( size_t mip = 0; mip < meta.mipLevels; ++mip )
	{
		for ( size_t item = 0; item < meta.arraySize; ++item )
		{
			size_t tIdx = meta.ComputeIndex( mip, item, 0 );
			auto subresources = image.GetImages();

			uint32 rowPitch = static_cast<uint32>( subresources[tIdx].rowPitch );
			uint32 slicePitch = static_cast<uint32>( subresources[tIdx].slicePitch );
			uint32 offset = static_cast<uint32>( subresources[tIdx].pixels - initializer.m_memory );

			initializer.m_sections.emplace_back( rowPitch, slicePitch, offset );
		}
	}

	return initializer;
}