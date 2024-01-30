#include "GpuProfiler.h"

#include "GraphicsApiResource.h"
#include "Multithread/TaskScheduler.h"
#include "Query.h"
#include "SizedTypes.h"

#include <cassert>
#include <map>
#include <stack>

namespace rendercore
{
	class GpuProfiler : public IGpuProfiler
	{
	public:
		virtual void RegisterProfile( GpuProfileData& profileData ) override;

		virtual void StartProfile( CommandList& commandList, GpuProfileData& profileData ) override;
		virtual void EndProfile( CommandList& commandList, GpuProfileData& profileData ) override;

		virtual void GatherProfileData() override;

		virtual const std::vector<GpuProfileData*>& GetProfileDatas() const override;

		void CleanUp();

		GpuProfiler();

	private:
		std::vector<GpuProfileData*> m_profiles;
		std::vector<agl::RefHandle<agl::GpuTimer>> m_gpuTimers[GpuProfileData::TimerLatency];

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

		m_profileStack.pop();
	}

	void GpuProfiler::GatherProfileData()
	{
		assert( IsInRenderThread() );

		uint32 nextFrame = ( m_curFrame + 1 ) % GpuProfileData::TimerLatency;
		for ( auto& profile : m_profiles )
		{
			if ( profile->m_queryEnded[nextFrame] == false )
			{
				continue;
			}

			double duration = profile->m_timers[nextFrame]->GetDuration();
			double total = profile->m_averageMS * profile->m_numSamples + duration;

			++profile->m_numSamples;

			profile->m_averageMS = total / profile->m_numSamples;

			profile->m_queryStarted[nextFrame] = false;
			profile->m_queryEnded[nextFrame] = false;
		}

		m_curFrame = nextFrame;
	}

	const std::vector<GpuProfileData*>& GpuProfiler::GetProfileDatas() const
	{
		return m_profiles;
	}

	void GpuProfiler::CleanUp()
	{
		for ( auto& gpuTimers : m_gpuTimers )
		{
			EnqueueRenderTask( [timers = std::move( gpuTimers )]() mutable
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
