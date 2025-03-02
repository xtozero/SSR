#include "RenderGraphEvent.h"

#include "RenderGraph.h"

namespace rendercore
{
	RenderGraphScopedGpuProfile::RenderGraphScopedGpuProfile( RenderGraph& renderGraph, GpuProfileData& gpuProfileData )
		: m_renderGraph( renderGraph )
	{
		m_renderGraph.PushGPUProfileEvent( gpuProfileData );
	}

	RenderGraphScopedGpuProfile::~RenderGraphScopedGpuProfile()
	{
		m_renderGraph.PopGPUProfileEvent();
	}

	RenderGraphScopedPipelineStat::RenderGraphScopedPipelineStat( RenderGraph& renderGraph, PipelineStatData& pipelineStatData )
		: m_renderGraph( renderGraph )
	{
		m_renderGraph.PushPipelineStateEvent( pipelineStatData );
	}

	RenderGraphScopedPipelineStat::~RenderGraphScopedPipelineStat()
	{
		m_renderGraph.PopPipelineStateEvent();
	}

	void BeginGpuProfileEvent( ComputeCommandList& commandList, RenderGraphPass& pass, GpuProfileRenderGraphEvent* event )
	{
		auto recursive = [&commandList, &pass]( GpuProfileRenderGraphEvent* cur, auto& recursive )
		{
			if ( ( cur == nullptr ) || ( &pass != cur->m_firstPass ) )
			{
				return;
			}

			recursive( cur->m_parent, recursive );

			GetGpuProfiler().BeginProfile( commandList, *cur->m_eventData );
		};

		recursive( event, recursive );
	}

	void EndGpuProfileEvent( ComputeCommandList& commandList, RenderGraphPass& pass, GpuProfileRenderGraphEvent* event )
	{
		for ( GpuProfileRenderGraphEvent* cur = event; cur != nullptr; cur = cur->m_parent )
		{
			if ( &pass != cur->m_lastPass )
			{
				break;
			}

			GetGpuProfiler().EndProfile( commandList, *cur->m_eventData );
		}
	}

	void BeginPipelineStatEvent( ComputeCommandList& commandList, RenderGraphPass& pass, PipelineStateRenderGraphEvent* event )
	{
		auto recursive = [&commandList, &pass]( PipelineStateRenderGraphEvent* cur, auto& recursive )
		{
			if ( ( cur == nullptr ) || ( &pass != cur->m_firstPass ) )
			{
				return;
			}

			recursive( cur->m_parent, recursive );

			GetGpuProfiler().BeginPipelineStat( commandList, *cur->m_eventData );
		};

		recursive( event, recursive );
	}

	void EndPipelineStatEvent( ComputeCommandList& commandList, RenderGraphPass& pass, PipelineStateRenderGraphEvent* event )
	{
		for ( PipelineStateRenderGraphEvent* cur = event; cur != nullptr; cur = cur->m_parent )
		{
			if ( &pass != cur->m_lastPass )
			{
				break;
			}

			GetGpuProfiler().EndPipelineStat( commandList, *cur->m_eventData );
		}
	}
}