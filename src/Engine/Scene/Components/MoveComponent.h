#pragma once

#include "Engine/Scene/Component.h"

namespace MyGameEngine
{
    class GameObject;

    class MoveComponent final : public Component
    {
    public:
        explicit MoveComponent(GameObject& owner) noexcept;

        void Update(float deltaTime) override;

        void SetVelocity(float x, float y, float z) noexcept;

    private:
        GameObject& owner_;
        float velocityX_ = 0.0f;
        float velocityY_ = 0.0f;
        float velocityZ_ = 0.0f;
    };
} // namespace MyGameEngine
