#pragma once

namespace agl
{
    class VulkanGpuTimer final : public GpuTimer
    {
    public:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        virtual void Begin( ICommandListBase& commandList ) override;
        virtual void End( ICommandListBase& commandList ) override;

        virtual double GetDuration() override;

    private:
    };

    class VulkanOcclusionTest final : public OcclusionQuery
    {
    public:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        virtual void Begin( ICommandListBase& commandList ) override;
        virtual void End( ICommandListBase& commandList ) override;

        virtual uint64 GetNumSamplePassed() override;
        virtual bool IsDataReady() const override;

    private:
    };

    class VulkanPipelineStatistics final : public PipelineStatistics
    {
    public:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        virtual void Begin( ICommandListBase& commandList ) override;
        virtual void End( ICommandListBase& commandList ) override;

        virtual PipelineStatisticsData GetStatisticsData() const override;

    private:
    };
}
