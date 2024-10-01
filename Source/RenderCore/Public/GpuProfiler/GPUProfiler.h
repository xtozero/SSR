#pragma once

#include "CommandList.h"
#include "GuideTypes.h"
#include "NameTypes.h"

#define ENABLE_GPU_PROFILE 1

namespace agl
{
	class GpuTimer;
}

namespace rendercore
{
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

		static constexpr int32 TimerLatency = 5;

		Name m_label;
		const GpuProfileData* m_parent = nullptr;
		const GpuProfileData* m_child = nullptr;
		const GpuProfileData* m_sibling = nullptr;

		agl::GpuTimer* m_timers[TimerLatency] = {};

		bool m_queryStarted[TimerLatency] = {};
		bool m_queryEnded[TimerLatency] = {};
		bool m_avaliable = false;

		uint64 m_numSamples = 0;

		static constexpr int32 MaxSamples = 20;
		double m_durationMS[MaxSamples] = {};

		RENDERCORE_DLL double CalcAverageMS() const;
		RENDERCORE_DLL bool IsAvaliable() const;
	};

	class IGpuProfiler
	{
	public:
		virtual void RegisterProfile( GpuProfileData& profileData ) = 0;

		virtual void StartProfile( CommandList& commandList, GpuProfileData& profileData ) = 0;
		virtual void EndProfile( CommandList& commandList, GpuProfileData& profileData ) = 0;

		virtual void BeginFrameRendering() = 0;
		virtual void GatherProfileData() = 0;

		virtual const std::vector<GpuProfileData*>& GetProfileData() const = 0;

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
}

