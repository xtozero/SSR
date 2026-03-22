#pragma once

#include "GraphicsApiResource.h"
#include "Math/Matrix3X4.h"

namespace agl
{
    struct BLASDesc
    {
        RefHandle<Buffer> m_vertexBuffer;
        RefHandle<Buffer> m_indexBuffer;
    };

    class BLAS : public GraphicsApiResource
    {
    public:
        AGL_DLL static RefHandle<BLAS> Create( const BLASDesc& desc, const char* debugName );

    protected:
        BLASDesc m_desc = {};
    };

    struct RaytracingInstanceDesc
    {
        Matrix3X4 m_worldTransform;
        uint32 m_instanceId;
        RefHandle<BLAS> m_blas;
    };

    struct TLASDesc
    {
        std::vector<RaytracingInstanceDesc> instanceDescs;
    };

    class TLAS : public GraphicsApiResource
    {
    public:
        AGL_DLL static RefHandle<TLAS> Create( const TLASDesc& desc, const char* debugName );

        virtual Buffer* Resource() const = 0;
    };
}
