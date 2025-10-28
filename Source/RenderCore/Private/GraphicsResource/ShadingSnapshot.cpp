#include "ShadingSnapshot.h"

namespace rendercore
{
    int32 ShadingSnapshotBucket::Add( const ShadingSnapshot& snapshot )
    {
        constexpr SharedSnapshotId Dummy( 0 );
        auto [iter, success] = m_bucket.emplace( snapshot, Dummy );
        if ( success )
        {
            size_t id = m_snapshots.Add( snapshot );
            iter->second.m_id = static_cast<int32>( id );
        }

        ++iter->second.m_ref;
        return iter->second.m_id;
    }

    void ShadingSnapshotBucket::Remove( int32 id )
    {
        size_t index = static_cast<size_t>( id );
        const ShadingSnapshot& snapshot = m_snapshots[index];
        auto found = m_bucket.find( snapshot );
        if ( found == std::end( m_bucket ) )
        {
            // Error
            assert( false );
            return;
        }

        --found->second.m_ref;
        if ( found->second.m_ref == 0 )
        {
            m_bucket.erase( found );
            m_snapshots.RemoveAt( index );
        }
    }

    ShadingSnapshot& ShadingSnapshotBucket::Get( int32 id )
    {
        size_t index = static_cast<size_t>( id );
        assert( ( index < m_snapshots.GetMaxIndex() ) && m_snapshots.IsAllocated( index ) );

        return m_snapshots[index];
    }
}
