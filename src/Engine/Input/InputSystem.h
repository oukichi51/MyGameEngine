#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace MyGameEngine
{
    class InputSystem final
    {
    public:
        static constexpr std::size_t KeyCount = 256;

        InputSystem() = default;

        InputSystem(const InputSystem&) = delete;
        InputSystem& operator=(const InputSystem&) = delete;

        void Update() noexcept;
        void Reset() noexcept;

        [[nodiscard]] bool IsKeyDown(std::uint32_t keyCode) const noexcept;
        [[nodiscard]] bool IsKeyPressed(std::uint32_t keyCode) const noexcept;
        [[nodiscard]] bool IsKeyReleased(std::uint32_t keyCode) const noexcept;

    private:
        [[nodiscard]] static bool IsValidKeyCode(std::uint32_t keyCode) noexcept;

        std::array<bool, KeyCount> currentKeyStates_{};
        std::array<bool, KeyCount> previousKeyStates_{};
    };
} // namespace MyGameEngine
