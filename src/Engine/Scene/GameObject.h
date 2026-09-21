#pragma once

#include "Engine/Scene/Component.h"
#include "Engine/Scene/Transform.h"

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace MyGameEngine
{
    class GameObject final
    {
    public:
        GameObject() = default;
        ~GameObject() = default;

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = delete;
        GameObject& operator=(GameObject&&) = delete;

        Transform& GetTransform() noexcept;
        const Transform& GetTransform() const noexcept;

        void Update(float deltaTime);

        template <typename T, typename... Args>
            requires std::derived_from<T, Component>
        T& AddComponent(Args&&... args)
        {
            auto component = std::make_unique<T>(*this, std::forward<Args>(args)...);
            T& componentReference = *component;
            components_.push_back(std::move(component));
            return componentReference;
        }

        template <typename T>
            requires std::derived_from<T, Component>
        T* GetComponent() noexcept
        {
            for (const auto& component : components_)
            {
                if (T* matchedComponent = dynamic_cast<T*>(component.get()))
                {
                    return matchedComponent;
                }
            }

            return nullptr;
        }

        template <typename T>
            requires std::derived_from<T, Component>
        const T* GetComponent() const noexcept
        {
            for (const auto& component : components_)
            {
                if (const T* matchedComponent = dynamic_cast<const T*>(component.get()))
                {
                    return matchedComponent;
                }
            }

            return nullptr;
        }

    private:
        Transform transform_;
        std::vector<std::unique_ptr<Component>> components_;
    };
} // namespace MyGameEngine
