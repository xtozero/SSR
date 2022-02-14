#pragma once

#include "PassProcessor.h"

namespace rendercore
{
	class VolumetricCloundDrawPassProcessor : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};
}