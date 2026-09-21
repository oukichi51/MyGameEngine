#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "Engine/Scene/Components/MoveComponent.h"
#include "Engine/Scene/GameObject.h"

namespace MyGameEngine
{
    class Scene final
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) noexcept = default;
        Scene& operator=(Scene&&) noexcept = default;

        GameObject& CreateGameObject();
        bool DestroyGameObject(const GameObject& gameObject);

        void Update(float deltaTime);
        void Clear() noexcept;

        [[nodiscard]] std::size_t GetGameObjectCount() const noexcept;

    private:
        std::vector<std::unique_ptr<GameObject>> gameObjects_;
    };
} // namespace MyGameEngine
