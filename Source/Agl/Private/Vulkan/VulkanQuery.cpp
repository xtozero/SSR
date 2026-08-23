#include "VulkanQuery.h"

namespace agl
{
    void VulkanGpuTimer::InitResource()
    {
    }

    void VulkanGpuTimer::FreeResource()
    {
    }

    void VulkanGpuTimer::Begin( ICommandListBase& commandList )
    {
    }

    void VulkanGpuTimer::End( ICommandListBase& commandList )
    {
    }

    double VulkanGpuTimer::GetDuration()
    {
        return 0;
    }

    void VulkanOcclusionTest::InitResource()
    {
    }

    void VulkanOcclusionTest::FreeResource()
    {
    }

    void VulkanOcclusionTest::Begin( ICommandListBase& commandList )
    {
    }

    void VulkanOcclusionTest::End( ICommandListBase& commandList )
    {
    }

    uint64 VulkanOcclusionTest::GetNumSamplePassed()
    {
        return 0;
    }

    bool VulkanOcclusionTest::IsDataReady() const
    {
        return true;
    }

    void VulkanPipelineStatistics::InitResource()
    {
    }

    void VulkanPipelineStatistics::FreeResource()
    {
    }

    void VulkanPipelineStatistics::Begin( ICommandListBase& commandList )
    {
    }

    void VulkanPipelineStatistics::End( ICommandListBase& commandList )
    {
    }

    PipelineStatisticsData VulkanPipelineStatistics::GetStatisticsData() const
    {
        return {};
    }
}