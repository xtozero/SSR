#pragma once

#include "PassProcessor.h"

namespace rendercore
{
	class HitProxyPassProcessor final : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;

	protected:
		virtual PassShader CollectPassShader( MaterialResource& material ) const override;
	};
}
