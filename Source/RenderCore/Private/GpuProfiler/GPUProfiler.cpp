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
	double GpuProfileData::CalcAverageMS() const
	{
		return std::accumulate( std::begin( m_durationMS ), std::end( m_durationMS ), 0. ) / MaxSamples;
	}

	RENDERCORE_DLL bool GpuProfileData::IsAvaliable() const
	{
		return m_avaliable;
	}

	class GpuProfiler : public IGpuProfiler
	{
	public:
		virtual void RegisterProfile( GpuProfileData& profileData ) override;

		virtual void StartProfile( CommandList& commandList, GpuProfileData& profileData ) override;
		virtual void EndProfile( CommandList& commandList, GpuProfileData& profileData ) override;

		virtual void BeginFrameRendering() override;
		virtual void GatherProfileData() override;

		virtual const std::vector<GpuProfileData*>& GetProfileData() const override;

		void CleanUp();

		GpuProfiler();

	private:
		std::vector<GpuProfileData*> m_profiles;
		std::vector<RefHandle<agl::GpuTimer>> m_gpuTimers[GpuProfileData::TimerLatency];

		uint32 m_curFrame = 0;

		std::stack<GpuProfileData*> m_profileStack;
	};

	void GpuProfiler::RegisterProfile( GpuProfileData& profileData )
	{
		assert( IsInRenderThread() );
		m_profiles.push_back( &profileData );
	}

	void GpuProfiler::StartProfile( CommandList& commandList, GpuProfileData& profileData )
	{
		assert( IsInRenderThread() );
		assert( profileData.m_queryStarted[m_curFrame] == false);
		assert( profileData.m_queryEnded[m_curFrame] == false );

		if ( profileData.m_timers[m_curFrame] == nullptr )
		{
			auto gpuTimer = agl::GpuTimer::Create();
			gpuTimer->Init();
			profileData.m_timers[m_curFrame] = gpuTimer.Get();
			m_gpuTimers[m_curFrame].emplace_back( std::move( gpuTimer ) );
		}

		commandList.BeginQuery( profileData.m_timers[m_curFrame] );
		profileData.m_queryStarted[m_curFrame] = true;

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
		assert( profileData.m_queryStarted[m_curFrame] == true );
		assert( profileData.m_queryEnded[m_curFrame] == false );

		commandList.EndQuery( profileData.m_timers[m_curFrame] );
		profileData.m_queryEnded[m_curFrame] = true;
		profileData.m_avaliable = true;

		m_profileStack.pop();
	}

	void GpuProfiler::BeginFrameRendering()
	{
		for ( auto& profileData : m_profiles )
		{
			++profileData->m_numSamples;

			int32 durationIdx = profileData->m_numSamples % GpuProfileData::MaxSamples;
			profileData->m_durationMS[durationIdx] = 0;
		}
	}

	void GpuProfiler::GatherProfileData()
	{
		assert( IsInRenderThread() );

		uint32 nextFrame = ( m_curFrame + 1 ) % GpuProfileData::TimerLatency;
		for ( auto& profileData : m_profiles )
		{
			if ( profileData->m_queryEnded[nextFrame] == false )
			{
				profileData->m_avaliable = false;
				continue;
			}

			double duration = profileData->m_timers[nextFrame]->GetDuration();

			int32 durationIdx = profileData->m_numSamples % GpuProfileData::MaxSamples;
			profileData->m_durationMS[durationIdx] += duration;

			profileData->m_queryStarted[nextFrame] = false;
			profileData->m_queryEnded[nextFrame] = false;
		}

		m_curFrame = nextFrame;
	}

	const std::vector<GpuProfileData*>& GpuProfiler::GetProfileData() const
	{
		return m_profiles;
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
	}

	GpuProfiler::GpuProfiler()
	{
		m_profiles.reserve( 128 );
		for ( auto& gpuTimers : m_gpuTimers )
		{
			gpuTimers.reserve( 128 );
		}
	}

	ScopedGpuProfile::ScopedGpuProfile( CommandList& commandList, GpuProfileData& gpuProfileData )
		: m_commandList( commandList )
		, m_gpuProfileData( gpuProfileData )
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
