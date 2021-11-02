#pragma once

#include "PassProcessor.h"

class DefaultPassProcessor : public IPassProcessor
{
public:
	virtual std::optional<DrawSnapshot> Process( const PrimitiveProxy& proxy, const PrimitiveSubMesh& subMesh ) override;
};