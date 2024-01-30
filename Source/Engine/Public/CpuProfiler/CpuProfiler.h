#pragma once

#include "common.h"
#include "GuideTypes.h"
#include "NameTypes.h"

#include <chrono>
#include <thread>

#define ENABLE_CPU_PROFILE 1

namespace engine
{
	struct CpuProfileData
	{
		explicit CpuProfileData( const char* label )
			: m_label( label )
		{}

		Name m_label;

		const CpuProfileData* m_parent = nullptr;
		const CpuProfileData* m_child = nullptr;
		const CpuProfileData* m_sibling = nullptr;

		std::chrono::time_point<std::chrono::steady_clock> m_timeStampBegin;
		std::chrono::time_point<std::chrono::steady_clock> m_timeStampEnd;

		double m_averageMS = 0;
		uint64 m_numSamples = 0;

		std::thread::id m_threadId;
	};

	class ICpuProfiler
	{
	public:
		virtual void StartProfile( CpuProfileData& profileData ) = 0;
		virtual void EndProfile( CpuProfileData& profileData ) = 0;

		virtual void GetProfileDatas( std::thread::id threadId, std::vector<const CpuProfileData*>& outProfileData ) const = 0;

		virtual ~ICpuProfiler() = default;
	};

	class ScopedCpuProfile
	{
	public:
		ENGINE_DLL ScopedCpuProfile( CpuProfileData& cpuProfileData );
		ENGINE_DLL ~ScopedCpuProfile();

	private:
		CpuProfileData& m_cpuProfileData;
	};

	Owner<ICpuProfiler*> CreateCpuProfiler();
	void DestroyCpuProfiler( Owner<ICpuProfiler*> cpuProfiler );
}

#if ENABLE_CPU_PROFILE
#define CPU_PROFILE( name ) \
	thread_local engine::CpuProfileData CpuProfileData_##name( #name ); \
	engine::ScopedCpuProfile ScopedCpuProfile_##name( CpuProfileData_##name );
#else
#define CPU_PROFILE( name )
#endif