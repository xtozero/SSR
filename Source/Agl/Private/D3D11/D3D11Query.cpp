#include "D3D11Query.h"

#include "D3D11Api.h"
#include "ICommandList.h"

namespace agl
{
	void D3D11GpuTimer::InitResource()
	{
		D3D11_QUERY_DESC desc = {
			.Query = D3D11_QUERY_TIMESTAMP,
			.MiscFlags = 0,
		};

		D3D11Device().CreateQuery( &desc, &m_timeStampBegin );
		D3D11Device().CreateQuery( &desc, &m_timeStampEnd );

		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

		D3D11Device().CreateQuery( &desc, &m_timeStampDisjoint );
	}

	void D3D11GpuTimer::FreeResource()
	{
		if ( m_timeStampBegin )
		{
			m_timeStampBegin->Release();
			m_timeStampBegin = nullptr;
		}

		if ( m_timeStampEnd )
		{
			m_timeStampEnd->Release();
			m_timeStampEnd = nullptr;
		}

		if ( m_timeStampDisjoint )
		{
			m_timeStampDisjoint->Release();
			m_timeStampDisjoint = nullptr;
		}
	}

	void D3D11GpuTimer::Begin( ICommandListBase& commandList )
	{
		commandList.BeginQuery( m_timeStampDisjoint );
		commandList.EndQuery( m_timeStampBegin );
	}

	void D3D11GpuTimer::End( ICommandListBase& commandList )
	{
		commandList.EndQuery( m_timeStampEnd );
		commandList.EndQuery( m_timeStampDisjoint );
	}

	double D3D11GpuTimer::GetDuration()
	{
		uint64 beginTime = 0;
		while ( D3D11Context().GetData( m_timeStampBegin, &beginTime, sizeof( beginTime ), 0 ) != S_OK );

		uint64 endTime = 0;
		while ( D3D11Context().GetData( m_timeStampEnd, &endTime, sizeof( endTime ), 0 ) != S_OK );

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData = {};
		while ( D3D11Context().GetData( m_timeStampDisjoint, &disjointData, sizeof( disjointData ), 0 ) != S_OK );

		if ( disjointData.Disjoint == false )
		{
			auto deltaTime = static_cast<double>( endTime - beginTime );
			auto frequency = static_cast<double>( disjointData.Frequency );

			return deltaTime / frequency * 1000.0; // millisecond
		}

		return 0.0;
	}

	void D3D11OcclusionTest::InitResource()
	{
		D3D11_QUERY_DESC desc = {
			.Query = D3D11_QUERY_OCCLUSION,
			.MiscFlags = 0,
		};

		D3D11Device().CreateQuery( &desc, &m_occlusionTest );
	}

	void D3D11OcclusionTest::FreeResource()
	{
		if ( m_occlusionTest )
		{
			m_occlusionTest->Release();
			m_occlusionTest = nullptr;
		}
	}

	void D3D11OcclusionTest::Begin( ICommandListBase& commandList )
	{
		commandList.BeginQuery( m_occlusionTest );
	}

	void D3D11OcclusionTest::End( ICommandListBase& commandList )
	{
		commandList.EndQuery( m_occlusionTest );
	}

	uint64 D3D11OcclusionTest::GetNumSamplePassed()
	{
		uint64 numSamplePassed = std::numeric_limits<uint64>::max();
		while ( D3D11Context().GetData( m_occlusionTest, &numSamplePassed, sizeof( numSamplePassed ), 0 ) != S_OK );

		return numSamplePassed;
	}

	bool D3D11OcclusionTest::IsDataReady() const
	{
		return true;
	}
}