#include "Engine/Core/TimeSystem.h"

namespace MyGameEngine
{
    TimeSystem::TimeSystem() noexcept
    {
        Reset();
    }

    void TimeSystem::Update() noexcept
    {
        const Clock::time_point currentTime = Clock::now();
        deltaTime_ = std::chrono::duration<float>(currentTime - previousTime_).count();
        totalTime_ = std::chrono::duration<double>(currentTime - startTime_).count();
        previousTime_ = currentTime;
    }

    void TimeSystem::Reset() noexcept
    {
        startTime_ = Clock::now();
        previousTime_ = startTime_;
        deltaTime_ = 0.0f;
        totalTime_ = 0.0;
    }

    float TimeSystem::GetDeltaTime() const noexcept
    {
        return deltaTime_;
    }

    double TimeSystem::GetTotalTime() const noexcept
    {
        return totalTime_;
    }
} // namespace MyGameEngine
