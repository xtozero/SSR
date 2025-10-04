#pragma once

namespace rendercore
{
    class RenderGraph;

    class AsyncComputeTestBed
    {
    public:
        void IndependentHeavyWork( RenderGraph& renderGraph );
    };
}