#pragma once

#include "Engine/Scene/Scene.h"

#include <memory>

namespace MyGameEngine
{
    class SceneManager final
    {
    public:
        SceneManager() = default;
        ~SceneManager() = default;

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&) noexcept = default;
        SceneManager& operator=(SceneManager&&) noexcept = default;

        void SetScene(std::unique_ptr<Scene> scene);
        void Update(float deltaTime);

        Scene* GetCurrentScene() noexcept;
        const Scene* GetCurrentScene() const noexcept;

    private:
        std::unique_ptr<Scene> currentScene_;
    };
} // namespace MyGameEngine
