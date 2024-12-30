#pragma once

#include "GraphicsApiResource.h"
#include "LibraryTool/Common.h"

namespace agl
{
	class ICommandListBase;

	class Query : public GraphicsApiResource
	{
	public:
		virtual void Begin( ICommandListBase& commandList ) = 0;
		virtual void End( ICommandListBase& commandList ) = 0;
	};

	class GpuTimer : public Query
	{
	public:
		AGL_DLL static RefHandle<GpuTimer> Create();

		virtual double GetDuration() = 0;
	};

	class OcclusionQuery : public Query
	{
	public:
		AGL_DLL static RefHandle<OcclusionQuery> Create();

		virtual uint64 GetNumSamplePassed() = 0;
		virtual bool IsDataReady() const = 0;
	};

	class PipelineStatisticsData
	{
	public:
		uint64 m_verticesIA = 0;
		uint64 m_primitivesIA = 0;
		uint64 m_invocationsVS = 0;
		uint64 m_invocationsGS = 0;
		uint64 m_primitivesGS = 0;
		uint64 m_invocationsC = 0;
		uint64 m_primitivesC = 0;
		uint64 m_invocationsPS = 0;
		uint64 m_invocationsHS = 0;
		uint64 m_invocationsDS = 0;
		uint64 m_invocationsCS = 0;
		uint64 m_invocationsAS = 0;
		uint64 m_invocationsMS = 0;
		uint64 m_primitivesMS = 0;

		PipelineStatisticsData& operator+=( const PipelineStatisticsData& other )
		{
			m_verticesIA += other.m_verticesIA;
			m_primitivesIA += other.m_primitivesIA;
			m_invocationsVS += other.m_invocationsVS;
			m_invocationsGS += other.m_invocationsGS;
			m_primitivesGS += other.m_primitivesGS;
			m_invocationsC += other.m_invocationsC;
			m_primitivesC += other.m_primitivesC;
			m_invocationsPS += other.m_invocationsPS;
			m_invocationsHS += other.m_invocationsHS;
			m_invocationsDS += other.m_invocationsDS;
			m_invocationsCS += other.m_invocationsCS;
			m_invocationsAS += other.m_invocationsAS;
			m_invocationsMS += other.m_invocationsMS;
			m_primitivesMS += other.m_primitivesMS;

			return *this;
		}
	};

	class PipelineStatistics : public Query
	{
	public:
		AGL_DLL static RefHandle<PipelineStatistics> Create();

		virtual PipelineStatisticsData GetStatisticsData() const = 0;
	};
}