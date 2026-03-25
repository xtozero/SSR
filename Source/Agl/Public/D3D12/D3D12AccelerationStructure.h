#pragma once

#include "AccelerationStructure.h"
#include "D3D12Buffer.h"

namespace agl
{
    class D3D12BLAS : public BLAS
    {
    public:
        virtual void Build( IComputeCommandList& commandList ) override;

        ID3D12Resource* Resource();

        D3D12BLAS( const BLASDesc& desc, const char* debugName );

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        BLASDesc m_desc;

        uint64 m_scratchDataSizeInBytes = 0;
        RefHandle<D3D12Buffer> m_blas;
    };

    class D3D12TLAS : public TLAS
    {
    public:
        virtual void Build( IComputeCommandList& commandList ) override;
        virtual Buffer* Resource() const override;

        D3D12TLAS( const TLASDesc& desc, const char* debugName );

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        TLASDesc m_desc;

        uint64 m_scratchDataSizeInBytes = 0;
        RefHandle<D3D12Buffer> m_tlas;
    };
}