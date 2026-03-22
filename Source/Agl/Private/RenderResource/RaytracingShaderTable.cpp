#include "RaytracingShaderTable.h"

namespace agl
{
    size_t HitGroupDesc::GetHash() const
    {
        static size_t typeHash = typeid( HitGroupDesc ).hash_code();
        size_t hash = typeHash;

        HashCombine( hash, m_intersection );
        HashCombine( hash, m_anyHit );
        HashCombine( hash, m_closestHit );

        return hash;
    }

    size_t RaytracingShaderTableDesc::GetHash() const
    {
        static size_t typeHash = typeid( RaytracingShaderTableDesc ).hash_code();
        size_t hash = typeHash;

        HashCombine( hash, m_rayGeneration );
        for ( const auto& hitGroupDesc : m_hitGroupDescs )
        {
            HashCombine( hash, hitGroupDesc.GetHash() );
        }

        for ( const auto& miss : m_misses )
        {
            HashCombine( hash, miss );
        }

        return hash;
    }
}
