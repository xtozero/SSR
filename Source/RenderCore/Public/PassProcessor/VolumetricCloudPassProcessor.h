#pragma once

#include "PassProcessor.h"

namespace rendercore
{
	class VolumetricCloundDrawPassProcessor final : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};
}