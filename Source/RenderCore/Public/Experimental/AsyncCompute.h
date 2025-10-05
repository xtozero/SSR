#pragma once

namespace rendercore
{
    class RenderGraph;

    class AsyncComputeTestBed
    {
    public:
        static void IndependentHeavyWork( RenderGraph& renderGraph );
        static void DependentHeavyWork( RenderGraph& renderGraph );
    };
}