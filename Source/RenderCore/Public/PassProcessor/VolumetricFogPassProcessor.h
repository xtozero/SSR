#pragma once

#include "PassProcessor.h"

namespace rendercore
{
	class VolumetricFogDrawPassProcessor : public IPassProcessor
	{
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};
}