#include "CpuProfiler.h"

#include "InterfaceFactories.h"

#include <cassert>
#include <map>
#include <mutex>
#include <numeric>
#include <set>
#include <shared_mutex>
#include <stack>

using ::std::chrono::duration;
using ::std::chrono::seconds;
using ::std::chrono::nanoseconds;
using ::std::chrono::steady_clock;
using ::std::chrono::time_point;

namespace engine
{
	thread_local std::set<CpuProfileData*>* LocalProfileDatas = nullptr;

	double CpuProfileData::CalcAverageMS() const
	{
		return std::accumulate( std::begin( m_durationMS ), std::end( m_durationMS ), 0. ) / MaxSamples;
	}

	bool CpuProfileData::IsAvaliable() const
	{
		uint64 currentTick = 0;
		if ( auto cpuProfiler = GetInterface<ICpuProfiler>() )
		{
			currentTick = cpuProfiler->GetTickCount();
		}

		constexpr uint64 ValidInterval = 5;
		return ( currentTick - m_lastCollectTick ) < ValidInterval;
	}

	class CpuProfiler : public ICpuProfiler
	{
	public:
		virtual void StartProfile( CpuProfileData& profileData ) override;
		virtual void EndProfile( CpuProfileData& profileData ) override;

		virtual void GetProfileData( std::thread::id threadId, std::vector<const CpuProfileData*>& outProfileData ) const override;

		virtual void Tick() override;
		virtual uint64 GetTickCount() const override;

	private:
		thread_local static std::stack<CpuProfileData*> m_profileStack;

		mutable std::shared_mutex m_profileDataLock;
		std::map<std::thread::id, std::set<CpuProfileData*>> m_profileDatas;

		steady_clock::time_point m_lastTickTime;
		int64 m_tickCount = 0;
	};

	thread_local std::stack<CpuProfileData*> CpuProfiler::m_profileStack;

	void CpuProfiler::StartProfile( CpuProfileData& profileData )
	{
		profileData.m_timeStampBegin = steady_clock::now();
		auto threadId = std::this_thread::get_id();

		profileData.m_threadId = threadId;

		if ( m_profileStack.empty() == false )
		{
			if ( profileData.m_parent == nullptr )
			{
				CpuProfileData* parent = m_profileStack.top();
				profileData.m_parent = parent;
				profileData.m_sibling = parent->m_child;
				parent->m_child = &profileData;
			}
		}
		else
		{
			{
				if ( LocalProfileDatas == nullptr )
				{
					std::unique_lock<std::shared_mutex> writeLock( m_profileDataLock );
					m_profileDatas.emplace( threadId, std::set<CpuProfileData*>() );
					LocalProfileDatas = &m_profileDatas[threadId];
				}

				LocalProfileDatas->emplace( &profileData );
			}
		}

		m_profileStack.push( &profileData );
	}

	void CpuProfiler::EndProfile( CpuProfileData& profileData )
	{
		profileData.m_timeStampEnd = steady_clock::now();
		assert( profileData.m_threadId == std::this_thread::get_id() );

		nanoseconds durationNS = profileData.m_timeStampEnd - profileData.m_timeStampBegin;
		double durationMS = duration_cast<duration<double>>( durationNS ).count() * 1000.0;

		int32 durationIdx = profileData.m_numSamples % CpuProfileData::MaxSamples;
		profileData.m_durationMS[durationIdx] = durationMS;

		++profileData.m_numSamples;

		profileData.m_lastCollectTick = m_tickCount;

		m_profileStack.pop();
	}

	void CpuProfiler::GetProfileData( std::thread::id threadId, std::vector<const CpuProfileData*>& outProfileData ) const
	{
		std::shared_lock<std::shared_mutex> writeLock( m_profileDataLock );
		if ( auto found = m_profileDatas.find( threadId ); found != std::end( m_profileDatas ) )
		{
			outProfileData.reserve( found->second.size() );
			for ( const CpuProfileData* profileData : found->second )
			{
				outProfileData.emplace_back( profileData );
			}
		}
	}

	void CpuProfiler::Tick()
	{
		auto current = steady_clock::now();
		auto duration = duration_cast<seconds>( current - m_lastTickTime );

		if ( duration > seconds( 1 ) )
		{
			m_lastTickTime = current;
			++m_tickCount;
		}
	}

	uint64 CpuProfiler::GetTickCount() const
	{
		return m_tickCount;
	}

	ScopedCpuProfile::ScopedCpuProfile( CpuProfileData& cpuProfileData )
		: m_cpuProfileData( cpuProfileData )
	{
		if ( auto cpuProfiler = GetInterface<ICpuProfiler>() )
		{
			cpuProfiler->StartProfile( m_cpuProfileData );
		}
	}

	ScopedCpuProfile::~ScopedCpuProfile()
	{
		if ( auto cpuProfiler = GetInterface<ICpuProfiler>() )
		{
			cpuProfiler->EndProfile( m_cpuProfileData );
		}
	}

	Owner<ICpuProfiler*> CreateCpuProfiler()
	{
		return new CpuProfiler();
	}

	void DestroyCpuProfiler( Owner<ICpuProfiler*> cpuProfiler )
	{
		delete cpuProfiler;
	}
}