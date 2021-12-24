#pragma once

#include "PassProcessor.h"

class ShadowDepthPassProcessor : public IPassProcessor
{
public:
	virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
};