#include "CpuProfiler.h"

#include "InterfaceFactories.h"

#include <cassert>
#include <map>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <stack>

using ::std::chrono::duration;
using ::std::chrono::nanoseconds;
using ::std::chrono::steady_clock;
using ::std::chrono::time_point;

namespace engine
{
	thread_local std::set<CpuProfileData*>* LocalProfileDatas = nullptr;

	class CpuProfiler : public ICpuProfiler
	{
	public:
		virtual void StartProfile( CpuProfileData& profileData ) override;
		virtual void EndProfile( CpuProfileData& profileData ) override;

		virtual void GetProfileDatas( std::thread::id threadId, std::vector<const CpuProfileData*>& outProfileData ) const override;

	private:
		thread_local static std::stack<CpuProfileData*> m_profileStack;

		mutable std::shared_mutex m_profileDataLock;
		std::map<std::thread::id, std::set<CpuProfileData*>> m_profileDatas;
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

		double total = profileData.m_averageMS * profileData.m_numSamples + durationMS;

		++profileData.m_numSamples;

		profileData.m_averageMS = total / profileData.m_numSamples;

		m_profileStack.pop();
	}

	void CpuProfiler::GetProfileDatas( std::thread::id threadId, std::vector<const CpuProfileData*>& outProfileData ) const
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