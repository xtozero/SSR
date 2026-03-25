#include "AccelerationStructure.h"

#include "IResourceManager.h"

namespace agl
{
    RefHandle<BLAS> BLAS::Create( const BLASDesc& desc, const char* debugName )
    {
        return GetInterface<IResourceManager>()->CreateBLAS( desc, debugName );
    }

    bool BLAS::IsBuilt() const
    {
        return m_blasBuilt;
    }

    RefHandle<TLAS> TLAS::Create( const TLASDesc& desc, const char* debugName )
    {
        return GetInterface<IResourceManager>()->CreateTLAS( desc, debugName );
    }
}
