#pragma once

#include "PassProcessor.h"

class DefaultPassProcessor : public IPassProcessor
{
public:
	virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
};