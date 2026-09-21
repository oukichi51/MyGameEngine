#include <windows.h>

#include "Engine/Scene/Components/MoveComponent.h"

#include "Engine/Scene/GameObject.h"

namespace MyGameEngine
{
    MoveComponent::MoveComponent(GameObject& owner) noexcept : owner_(owner)
    {
    }

    void MoveComponent::Update(float deltaTime)
    {
        const DirectX::XMFLOAT3 translation{
            velocityX_ * deltaTime,
            velocityY_ * deltaTime,
            velocityZ_ * deltaTime,
        };
        owner_.GetTransform().Translate(translation);

        // OutputDebugStringA("MoveComponent::Update\n");
    }

    void MoveComponent::SetVelocity(float x, float y, float z) noexcept
    {
        velocityX_ = x;
        velocityY_ = y;
        velocityZ_ = z;
    }
} // namespace MyGameEngine
