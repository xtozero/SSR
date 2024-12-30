#pragma once

#include "PassProcessor.h"

namespace rendercore
{
	class DefaultPassProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override;
	};
}
