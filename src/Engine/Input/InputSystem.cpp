#include "Engine/Input/InputSystem.h"

#include <Windows.h>

namespace MyGameEngine
{
    void InputSystem::Update() noexcept
    {
        previousKeyStates_ = currentKeyStates_;

        for (std::size_t keyCode = 0; keyCode < KeyCount; ++keyCode)
        {
            currentKeyStates_[keyCode] = (GetAsyncKeyState(static_cast<int>(keyCode)) & 0x8000) != 0;
        }
    }

    void InputSystem::Reset() noexcept
    {
        currentKeyStates_.fill(false);
        previousKeyStates_.fill(false);
    }

    bool InputSystem::IsKeyDown(std::uint32_t keyCode) const noexcept
    {
        return IsValidKeyCode(keyCode) && currentKeyStates_[keyCode];
    }

    bool InputSystem::IsKeyPressed(std::uint32_t keyCode) const noexcept
    {
        return IsValidKeyCode(keyCode) && currentKeyStates_[keyCode] && !previousKeyStates_[keyCode];
    }

    bool InputSystem::IsKeyReleased(std::uint32_t keyCode) const noexcept
    {
        return IsValidKeyCode(keyCode) && !currentKeyStates_[keyCode] && previousKeyStates_[keyCode];
    }

    bool InputSystem::IsValidKeyCode(std::uint32_t keyCode) noexcept
    {
        return keyCode < KeyCount;
    }
} // namespace MyGameEngine
