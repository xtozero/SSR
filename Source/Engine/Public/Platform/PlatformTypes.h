#pragma once

namespace engine
{
    struct PlatformWindowContext
    {
        void* m_nativeWindow = nullptr;
        void* m_nativeContext = nullptr;
    };
}