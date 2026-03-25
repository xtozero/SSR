#pragma once

#include "GraphicsApiResource.h"
#include "Math/Matrix3X4.h"

namespace agl
{
    class IComputeCommandList;

    struct BLASDesc
    {
        RefHandle<Buffer> m_vertexBuffer;
        RefHandle<Buffer> m_indexBuffer;
    };

    class BLAS : public GraphicsApiResource
    {
    public:
        AGL_DLL static RefHandle<BLAS> Create( const BLASDesc& desc, const char* debugName );

        virtual void Build( IComputeCommandList& commandList ) = 0;

        AGL_DLL bool IsBuilt() const;

    protected:
        BLASDesc m_desc = {};
        bool m_blasBuilt = false;
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

        virtual void Build( IComputeCommandList& commandList ) = 0;
        virtual Buffer* Resource() const = 0;
    };
}
