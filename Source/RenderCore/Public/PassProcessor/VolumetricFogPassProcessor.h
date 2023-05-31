#pragma once

#include "PassProcessor.h"

namespace rendercore
{
	class VolumetricFogDrawPassProcessor : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};
}