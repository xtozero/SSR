#pragma once

#include "Archive.h"
#include "Serializable.h"

class RawAsset : public Serializable
{
    GENERATE_CLASS_TYPE_INFO( RawAsset );

public:
    virtual void Serialize( Archive& ar ) override
    {
        if ( ar.IsWriteMode() )
        {
            ar.LoadFromMemory( m_data );
        }
        else
        {
            ar.WriteToMemory( m_data );
        }
    }

    explicit RawAsset( const BinaryChunk& data )
        : m_data( data )
    {}

private:
    PROPERTY( data )
    BinaryChunk m_data;
};