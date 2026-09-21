#include "Engine/Scene/GameObject.h"

namespace MyGameEngine
{
    Transform& GameObject::GetTransform() noexcept
    {
        return transform_;
    }

    const Transform& GameObject::GetTransform() const noexcept
    {
        return transform_;
    }

    void GameObject::Update(float deltaTime)
    {
        for (auto& component : components_)
        {
            component->Update(deltaTime);
        }
    }
} // namespace MyGameEngine
