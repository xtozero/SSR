#include "GpuProfiler.h"

#include "GraphicsApiResource.h"
#include "Multithread/TaskScheduler.h"
#include "Query.h"
#include "SizedTypes.h"

#include <cassert>
#include <map>
#include <numeric>
#include <stack>

namespace rendercore
{
	ScopedGpuEvent::ScopedGpuEvent( CommandList& commandList, const char* eventName )
		: m_commandList( commandList )
	{
		m_commandList.BeginEvent( eventName );
	}

	ScopedGpuEvent::~ScopedGpuEvent()
	{
		m_commandList.EndEvent();
	}

	double GpuProfileData::CalcAverageMS() const
	{
		return std::accumulate( std::begin( m_durationMS ), std::end( m_durationMS ), 0. ) / MaxSamples;
	}

	bool GpuProfileData::IsAvaliable() const
	{
		return m_avaliable;
	}

	const agl::PipelineStatisticsData& PipelineStatData::GetStatData() const
	{
		return m_statData[m_lastSampleIndex];
	}

	bool PipelineStatData::IsAvaliable() const
	{
		return m_avaliable;
	}

	class GpuProfiler : public IGpuProfiler
	{
	public:
		virtual void RegisterProfile( GpuProfileData& profileData ) override;
		virtual void RegisterPipelineStat( PipelineStatData& pipelineStatData ) override;

		virtual void StartProfile( CommandList& commandList, GpuProfileData& profileData ) override;
		virtual void EndProfile( CommandList& commandList, GpuProfileData& profileData ) override;

		virtual void StartPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData ) override;
		virtual void EndPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData ) override;

		virtual void BeginFrameRendering() override;
		virtual void GatherProfileData() override;

		virtual const std::vector<GpuProfileData*>& GetProfileData() const override;
		virtual const std::vector<PipelineStatData*>& GetPipelineStatData() const override;

		void CleanUp();

		GpuProfiler();

	private:
		std::vector<GpuProfileData*> m_profiles;
		std::vector<RefHandle<agl::GpuTimer>> m_gpuTimers[GpuProfilerDataLatency];

		std::vector<PipelineStatData*> m_pipelineStatDataList;
		std::vector<RefHandle<agl::PipelineStatistics>> m_pipelineStats[GpuProfilerDataLatency];

		uint32 m_curTick = 0;

		std::stack<GpuProfileData*> m_profileStack;
		std::stack<PipelineStatData*> m_pipelineStatStack;
	};

	void GpuProfiler::RegisterProfile( GpuProfileData& profileData )
	{
		assert( IsInRenderThread() );
		m_profiles.push_back( &profileData );
	}

	void GpuProfiler::RegisterPipelineStat( PipelineStatData& pipelineStatData )
	{
		assert( IsInRenderThread() );
		m_pipelineStatDataList.push_back( &pipelineStatData );
	}

	void GpuProfiler::StartProfile( CommandList& commandList, GpuProfileData& profileData )
	{
		assert( IsInRenderThread() );
		assert( profileData.m_queryStarted[m_curTick] == false);
		assert( profileData.m_queryEnded[m_curTick] == false );

		if ( profileData.m_timers[m_curTick] == nullptr )
		{
			auto gpuTimer = agl::GpuTimer::Create();
			gpuTimer->Init();
			profileData.m_timers[m_curTick] = gpuTimer.Get();
			m_gpuTimers[m_curTick].emplace_back( std::move( gpuTimer ) );
		}

		commandList.BeginQuery( profileData.m_timers[m_curTick] );
		profileData.m_queryStarted[m_curTick] = true;

		if ( ( m_profileStack.empty() == false ) && ( profileData.m_parent == nullptr ) )
		{
			GpuProfileData* parent = m_profileStack.top();
			profileData.m_parent = parent;
			profileData.m_sibling = parent->m_child;
			parent->m_child = &profileData;
		}

		m_profileStack.push( &profileData );
	}

	void GpuProfiler::EndProfile( CommandList& commandList, GpuProfileData& profileData )
	{
		assert( IsInRenderThread() );
		assert( profileData.m_queryStarted[m_curTick] == true );
		assert( profileData.m_queryEnded[m_curTick] == false );

		commandList.EndQuery( profileData.m_timers[m_curTick] );
		profileData.m_queryEnded[m_curTick] = true;
		profileData.m_avaliable = true;

		m_profileStack.pop();
	}

	void GpuProfiler::StartPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData )
	{
		assert( IsInRenderThread() );
		assert( pipelineStatData.m_queryStarted[m_curTick] == false );
		assert( pipelineStatData.m_queryEnded[m_curTick] == false );

		if ( pipelineStatData.m_stats[m_curTick] == nullptr )
		{
			auto pipelineStat = agl::PipelineStatistics::Create();
			pipelineStat->Init();
			pipelineStatData.m_stats[m_curTick] = pipelineStat.Get();
			m_pipelineStats[m_curTick].emplace_back( std::move( pipelineStat ) );
		}

		commandList.BeginQuery( pipelineStatData.m_stats[m_curTick] );
		pipelineStatData.m_queryStarted[m_curTick] = true;

		if ( ( m_pipelineStatStack.empty() == false ) && ( pipelineStatData.m_parent == nullptr ) )
		{
			PipelineStatData* parent = m_pipelineStatStack.top();
			pipelineStatData.m_parent = parent;
			pipelineStatData.m_sibling = parent->m_child;
			parent->m_child = &pipelineStatData;
		}

		m_pipelineStatStack.push( &pipelineStatData );
	}

	void GpuProfiler::EndPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData )
	{
		assert( IsInRenderThread() );
		assert( pipelineStatData.m_queryStarted[m_curTick] == true );
		assert( pipelineStatData.m_queryEnded[m_curTick] == false );

		commandList.EndQuery( pipelineStatData.m_stats[m_curTick] );
		pipelineStatData.m_queryEnded[m_curTick] = true;
		pipelineStatData.m_avaliable = true;

		m_pipelineStatStack.pop();
	}

	void GpuProfiler::BeginFrameRendering()
	{
		for ( auto& profileData : m_profiles )
		{
			++profileData->m_numSamples;

			int32 durationIndex = profileData->m_numSamples % GpuProfileData::MaxSamples;
			profileData->m_durationMS[durationIndex] = 0;
		}

		for ( auto& pipelineStatData : m_pipelineStatDataList )
		{
			++pipelineStatData->m_numSamples;

			int32 statDataIndex = pipelineStatData->m_numSamples % PipelineStatData::MaxSamples;
			pipelineStatData->m_statData[statDataIndex] = {};
		}
	}

	void GpuProfiler::GatherProfileData()
	{
		assert( IsInRenderThread() );

		uint32 nextTick = ( m_curTick + 1 ) % GpuProfilerDataLatency;
		for ( auto& profileData : m_profiles )
		{
			if ( profileData->m_queryEnded[nextTick] == false )
			{
				profileData->m_avaliable = false;
				continue;
			}

			double duration = profileData->m_timers[nextTick]->GetDuration();

			int32 durationIdx = profileData->m_numSamples % GpuProfileData::MaxSamples;
			profileData->m_durationMS[durationIdx] += duration;

			profileData->m_queryStarted[nextTick] = false;
			profileData->m_queryEnded[nextTick] = false;
		}

		for ( auto& pipelineStatData : m_pipelineStatDataList )
		{
			if ( pipelineStatData->m_queryEnded[nextTick] == false )
			{
				pipelineStatData->m_avaliable = false;
				continue;
			}

			agl::PipelineStatisticsData statData = pipelineStatData->m_stats[nextTick]->GetStatisticsData();

			int32 dataIndex = pipelineStatData->m_numSamples % PipelineStatData::MaxSamples;
			pipelineStatData->m_statData[dataIndex] += statData;
			pipelineStatData->m_lastSampleIndex = dataIndex;

			pipelineStatData->m_queryStarted[nextTick] = false;
			pipelineStatData->m_queryEnded[nextTick] = false;
		}

		m_curTick = nextTick;
	}

	const std::vector<GpuProfileData*>& GpuProfiler::GetProfileData() const
	{
		return m_profiles;
	}

	const std::vector<PipelineStatData*>& GpuProfiler::GetPipelineStatData() const
	{
		return m_pipelineStatDataList;
	}

	void GpuProfiler::CleanUp()
	{
		for ( auto& gpuTimers : m_gpuTimers )
		{
			EnqueueRenderTask(
				[timers = std::move( gpuTimers )]() mutable
				{
					for ( auto& timer : timers )
					{
						timer = nullptr;
					}
				} );
		}

		for ( auto& piplineStats : m_pipelineStats )
		{
			EnqueueRenderTask(
				[stats = std::move( piplineStats )]() mutable
				{
					for ( auto& stat : stats )
					{
						stat = nullptr;
					}
				} );
		}
	}

	GpuProfiler::GpuProfiler()
	{
		m_profiles.reserve( 128 );
		for ( auto& gpuTimers : m_gpuTimers )
		{
			gpuTimers.reserve( 128 );
		}

		m_pipelineStatDataList.reserve( 128 );
		for ( auto& pipelineStats : m_pipelineStats )
		{
			pipelineStats.reserve( 128 );
		}
	}

	ScopedGpuProfile::ScopedGpuProfile( CommandList& commandList, GpuProfileData& gpuProfileData )
		: m_commandList( commandList )
		, m_gpuProfileData( gpuProfileData )
		, m_gpuEvent( commandList, gpuProfileData.m_label.CStr() )
	{
		GetGpuProfiler().StartProfile( m_commandList, m_gpuProfileData );
	}

	ScopedGpuProfile::~ScopedGpuProfile()
	{
		GetGpuProfiler().EndProfile( m_commandList, m_gpuProfileData );
	}

	RegisterGpuProfileData::RegisterGpuProfileData( GpuProfileData& gpuProfileData )
	{
		GetGpuProfiler().RegisterProfile( gpuProfileData );
	}

	ScopedPipelineStat::ScopedPipelineStat( CommandList& commandList, PipelineStatData& pipelineStatData )
		: m_commandList( commandList )
		, m_pipelineStatData( pipelineStatData )
	{
		GetGpuProfiler().StartPipelineStat( m_commandList, m_pipelineStatData );
	}

	ScopedPipelineStat::~ScopedPipelineStat()
	{
		GetGpuProfiler().EndPipelineStat( m_commandList, m_pipelineStatData );
	}

	RegisterPipelineStatData::RegisterPipelineStatData( PipelineStatData& pipelineStatData )
	{
		GetGpuProfiler().RegisterPipelineStat( pipelineStatData );
	}

	IGpuProfiler& GetGpuProfiler()
	{
		static GpuProfiler gpuProfiler;
		return gpuProfiler;
	}

	void CleanUpGpuProfiler()
	{
		static_cast<GpuProfiler*>( &GetGpuProfiler() )->CleanUp();
	}
}
