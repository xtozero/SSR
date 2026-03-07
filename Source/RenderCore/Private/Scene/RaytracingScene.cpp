#include "Scene/RaytracingScene.h"

#include "PrimitiveProxy.h"
#include "RenderResource/AccelerationStructure.h"
#include "Scene/PrimitiveSceneInfo.h"

namespace rendercore
{
    void RaytracingScene::Build( RenderGraph& renderGraph )
    {
        if ( m_needsUpdate == false )
        {
            return;
        }

        if ( m_instances.Size() > 0 )
        {
            std::vector<agl::RaytracingInstanceDesc> instanceDescs;
            instanceDescs.reserve( m_instances.Size() );

            for ( const auto& instance : m_instances )
            {
                instanceDescs.emplace_back( instance );
            }

            agl::TLASDesc desc = {
                .instanceDescs = std::move( instanceDescs )
            };

            m_tlas = agl::TLAS::Create( desc, "RaytracingScene" );
            m_tlas->Init();
        }
        else
        {
            m_tlas = nullptr;
        }

        m_needsUpdate = false;
    }

    void RaytracingScene::AddInstance( PrimitiveSceneInfo& primitiveSceneInfo, RefHandle<agl::BLAS> blas )
    {
        if ( blas.Get() == nullptr )
        {
            return;
        }

        agl::RaytracingInstanceDesc instanceDesc = {
            .m_worldTransform = primitiveSceneInfo.Proxy()->WorldTransform().GetTrasposed(),
            .m_instanceId = primitiveSceneInfo.PrimitiveId(),
            .m_blas = blas,
        };

        primitiveSceneInfo.m_rayTracingInstanceId = static_cast<uint32>( m_instances.Add( instanceDesc ) );
        MarkInstanceDirty();
    }

    void RaytracingScene::RemoveInstance( PrimitiveSceneInfo& primitiveSceneInfo )
    {
        if ( primitiveSceneInfo.m_rayTracingInstanceId == InvalidInstanceId )
        {
            return;
        }

        m_instances.RemoveAt( primitiveSceneInfo.m_rayTracingInstanceId );
        MarkInstanceDirty();
    }

    void RaytracingScene::UpdateInstance( PrimitiveSceneInfo& primitiveSceneInfo )
    {
        if ( primitiveSceneInfo.m_rayTracingInstanceId == InvalidInstanceId )
        {
            return;
        }

        MarkInstanceDirty();
    }

    RefHandle<agl::TLAS> RaytracingScene::GetTLAS() const
    {
        return m_tlas;
    }

    void RaytracingScene::MarkInstanceDirty()
    {
        m_needsUpdate = true;
    }
}
