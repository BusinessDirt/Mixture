#pragma once

namespace Mixture::Vulkan
{
    struct FrameQueueActivity
    {
        bool Graphics = false;
        bool Transfer = false;
        bool Compute = false;
    };

    struct FrameSubmissionPlan
    {
        bool SubmitTransfer = false;
        bool SubmitCompute = false;
        bool WaitForTransfer = false;
        bool WaitForCompute = false;
    };

    inline FrameSubmissionPlan BuildFrameSubmissionPlan(const FrameQueueActivity& activity)
    {
        return { activity.Transfer, activity.Compute, activity.Transfer, activity.Compute };
    }
}
