#pragma once

#include "SceneRenderer.h"

namespace rendercore
{
    struct EditorOutlineRenderParams
    {
        IScene* m_scene = nullptr;
        RefHandle<agl::Texture> m_sceneColor;
        ResourceBinder* m_resourceBinder = nullptr;

        bool IsValid() const
        {
            return m_scene != nullptr
                && m_sceneColor.Get() != nullptr
                && m_resourceBinder != nullptr;
        }
    };

    class EditorOutlinePass
    {
    public:
        static void Render( RenderGraph& renderGraph, const EditorOutlineRenderParams& params );
    };
}
