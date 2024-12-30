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

	class ScopedGpuEvent
	{
	public:
		ScopedGpuEvent( CommandList& commandList, const char* eventName );
		~ScopedGpuEvent();

	private:
		CommandList& m_commandList;
	};

#if ENABLE_GPU_PROFILE
#define GPU_EVENT( commandList, name ) \
	ScopedGpuEvent ScopedGpuEvent_##name( commandList, #name );
#else
#define GPU_EVENT( commandList, name )
#endif

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

		virtual void StartProfile( CommandList& commandList, GpuProfileData& profileData ) = 0;
		virtual void EndProfile( CommandList& commandList, GpuProfileData& profileData ) = 0;

		virtual void StartPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData ) = 0;
		virtual void EndPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData ) = 0;

		virtual void BeginFrameRendering() = 0;
		virtual void GatherProfileData() = 0;

		virtual const std::vector<GpuProfileData*>& GetProfileData() const = 0;
		virtual const std::vector<PipelineStatData*>& GetPipelineStatData() const = 0;

		virtual ~IGpuProfiler() = default;
	};

	class ScopedGpuProfile
	{
	public:
		ScopedGpuProfile( CommandList& commandList, GpuProfileData& gpuProfileData );
		~ScopedGpuProfile();

	private:
		CommandList& m_commandList;
		GpuProfileData& m_gpuProfileData;

		ScopedGpuEvent m_gpuEvent;
	};

	class RegisterGpuProfileData
	{
	public:
		RegisterGpuProfileData( GpuProfileData& gpuProfileData );
	};

	class ScopedPipelineStat
	{
	public:
		ScopedPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData );
		~ScopedPipelineStat();

	private:
		CommandList& m_commandList;
		PipelineStatData& m_pipelineStatData;
	};

	class RegisterPipelineStatData
	{
	public:
		RegisterPipelineStatData( PipelineStatData& pipelineStatData );
	};

	IGpuProfiler& GetGpuProfiler();
	void CleanUpGpuProfiler();

#if ENABLE_GPU_PROFILE
#define GPU_PROFILE( commandList, name ) \
	static GpuProfileData GpuProfileData_##name( #name ); \
	static RegisterGpuProfileData RegisterGpuProfileData_##name( GpuProfileData_##name ); \
	ScopedGpuProfile ScopedGpuProfile_##name( commandList, GpuProfileData_##name );
#else
#define GPU_PROFILE( commandList, name )
#endif

#if ENABLE_PIPELINE_STATISTICS
#define PIPELINE_STAT( commandList, name ) \
	static PipelineStatData PipelineStatData_##name( #name ); \
	static RegisterPipelineStatData RegisterPipelineStatData_##name( PipelineStatData_##name ); \
	ScopedPipelineStat ScopedPipelineStat_##name( commandList, PipelineStatData_##name );
#else
#define PIPELINE_STAT( commandList, name )
#endif
}

