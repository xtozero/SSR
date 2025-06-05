#pragma once

#include "Archive.h"
#include "Serializable.h"

class RawAsset : public Serializable
{
    GENERATE_CLASS_TYPE_INFO( RawAsset );

public:
    explicit RawAsset( const BinaryChunk& data )
        : m_data( data )
    {}

private:
    PROPERTY( data )
    BinaryChunk m_data;
};