#pragma once

#include <chrono>

namespace MyGameEngine
{
    class TimeSystem final
    {
    public:
        TimeSystem() noexcept;

        TimeSystem(const TimeSystem&) = delete;
        TimeSystem& operator=(const TimeSystem&) = delete;

        void Update() noexcept;
        void Reset() noexcept;

        [[nodiscard]] float GetDeltaTime() const noexcept;
        [[nodiscard]] double GetTotalTime() const noexcept;

    private:
        using Clock = std::chrono::steady_clock;

        Clock::time_point startTime_;
        Clock::time_point previousTime_;
        float deltaTime_ = 0.0f;
        double totalTime_ = 0.0;
    };
} // namespace MyGameEngine
