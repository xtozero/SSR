#include "AccelerationStructure.h"

#include "IResourceManager.h"

namespace agl
{
    RefHandle<BLAS> BLAS::Create( const BLASDesc& desc, const char* debugName )
    {
        RefHandle<BLAS> newBLAS = GetInterface<IResourceManager>()->CreateBLAS( desc, debugName );
        EnqueueRenderTask(
            [blas = newBLAS]()
            {
                blas->Init();
            } );

        return newBLAS;
    }

    bool BLAS::IsBuilt() const
    {
        return m_blasBuilt;
    }

    RefHandle<TLAS> TLAS::Create( const TLASDesc& desc, const char* debugName )
    {
        RefHandle<TLAS> newTLAS = GetInterface<IResourceManager>()->CreateTLAS( desc, debugName );
        EnqueueRenderTask(
            [tlas = newTLAS]()
            {
                tlas->Init();
            } );

        return newTLAS;
    }
}
