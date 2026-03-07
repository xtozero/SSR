#include "AccelerationStructure.h"

#include "IRenderResourceManager.h"

namespace agl
{
    RefHandle<BLAS> BLAS::Create( const BLASDesc& desc, const char* debugName )
    {
        return GetInterface<IResourceManager>()->CreateBLAS( desc, debugName );
    }

    RefHandle<TLAS> TLAS::Create( const TLASDesc& desc, const char* debugName )
    {
        return GetInterface<IResourceManager>()->CreateTLAS( desc, debugName );
    }
}
