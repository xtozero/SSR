#pragma once

#include "PassProcessor.h"

class DepthWritePassProcessor : public IPassProcessor
{
public:
	virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
};