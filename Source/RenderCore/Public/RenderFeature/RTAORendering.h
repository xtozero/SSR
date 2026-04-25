#pragma once

#include "AccelerationStructure.h"
#include "CommonRenderResource.h"
#include "Texture.h"

namespace rendercore
{
    struct RTAORenderParams
    {
        RefHandle<agl::Buffer> m_viewShaderArguments;

        RefHandle<agl::TLAS> m_raytracingScene;

        RefHandle<agl::Texture> m_prevViewSpaceDistance;
        RefHandle<agl::Texture> m_viewSpaceDistance;
        RefHandle<agl::Texture> m_worldNormal;
        RefHandle<agl::Texture> m_velocity;

        uint32 m_screenWidth;
        uint32 m_screenHeight;

        bool IsValid() const
        {
            return m_viewShaderArguments.Get() != nullptr
                && m_raytracingScene.Get() != nullptr
                && m_prevViewSpaceDistance.Get() != nullptr
                && m_viewSpaceDistance.Get() != nullptr
                && m_worldNormal.Get() != nullptr
                && m_velocity.Get() != nullptr
                && m_screenWidth > 0
                && m_screenHeight > 0;
        }
    };

    class RTAORenderPass
    {
    public:
        RefHandle<agl::Texture> Render( RenderGraph& renderGraph, const RTAORenderParams& params );

    private:
        RefHandle<agl::Texture> m_prevAmbientOcclusion = WhiteTexture;
    };
}