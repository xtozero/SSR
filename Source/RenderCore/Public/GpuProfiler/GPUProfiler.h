#pragma once

#include "CommandList.h"
#include "GuideTypes.h"
#include "NameTypes.h"

#define ENABLE_GPU_PROFILE 1
#define ENABLE_PIPELINE_STATISTICS 1

namespace agl
{
	class GpuTimer;
	class PipelineStatistics;
}

namespace rendercore
{
	constexpr uint32 GpuProfilerDataLatency = 5;

	struct GpuProfileData
	{
		explicit GpuProfileData( const char* label )
			: m_label( label )
		{}

		Name m_label;
		const GpuProfileData* m_parent = nullptr;
		const GpuProfileData* m_child = nullptr;
		const GpuProfileData* m_sibling = nullptr;

		agl::GpuTimer* m_timers[GpuProfilerDataLatency] = {};

		bool m_queryStarted[GpuProfilerDataLatency] = {};
		bool m_queryEnded[GpuProfilerDataLatency] = {};
		bool m_avaliable = false;

		uint64 m_numSamples = 0;

		static constexpr int32 MaxSamples = 20;
		double m_durationMS[MaxSamples] = {};

		RENDERCORE_DLL double CalcAverageMS() const;
		RENDERCORE_DLL bool IsAvaliable() const;
	};

	struct PipelineStatData
	{
		explicit PipelineStatData( const char* label )
			: m_label( label )
		{
		}

		Name m_label;
		const PipelineStatData* m_parent = nullptr;
		const PipelineStatData* m_child = nullptr;
		const PipelineStatData* m_sibling = nullptr;

		agl::PipelineStatistics* m_stats[GpuProfilerDataLatency] = {};

		bool m_queryStarted[GpuProfilerDataLatency] = {};
		bool m_queryEnded[GpuProfilerDataLatency] = {};
		bool m_avaliable = false;

		uint64 m_numSamples = 0;
		int32 m_lastSampleIndex = 0;

		static constexpr int32 MaxSamples = 5;
		agl::PipelineStatisticsData m_statData[MaxSamples] = {};

		RENDERCORE_DLL const agl::PipelineStatisticsData& GetStatData() const;
		RENDERCORE_DLL bool IsAvaliable() const;
	};

	class IGpuProfiler
	{
	public:
		virtual void RegisterProfile( GpuProfileData& profileData ) = 0;
		virtual void RegisterPipelineStat( PipelineStatData& pipelineStatData ) = 0;

		virtual void BeginProfile( ComputeCommandList& commandList, GpuProfileData& profileData ) = 0;
		virtual void EndProfile( ComputeCommandList& commandList, GpuProfileData& profileData ) = 0;

		virtual void BeginPipelineStat( ComputeCommandList& commandList, PipelineStatData& pipelineStatData ) = 0;
		virtual void EndPipelineStat( ComputeCommandList& commandList, PipelineStatData& pipelineStatData ) = 0;

		virtual void BeginFrameRendering() = 0;
		virtual void GatherProfileData() = 0;

		virtual const std::vector<GpuProfileData*>& GetProfileData() const = 0;
		virtual const std::vector<PipelineStatData*>& GetPipelineStatData() const = 0;

		virtual ~IGpuProfiler() = default;
	};

	class RegisterGpuProfileData
	{
	public:
		explicit RegisterGpuProfileData( GpuProfileData& gpuProfileData );
	};

	class RegisterPipelineStatData
	{
	public:
		explicit RegisterPipelineStatData( PipelineStatData& pipelineStatData );
	};

	IGpuProfiler& GetGpuProfiler();
	void CleanUpGpuProfiler();
}

