#include "Engine/Scene/SceneManager.h"

#include <utility>

namespace MyGameEngine
{
    void SceneManager::SetScene(std::unique_ptr<Scene> scene)
    {
        currentScene_ = std::move(scene);
    }

    void SceneManager::Update(float deltaTime)
    {
        if (currentScene_ != nullptr)
        {
            currentScene_->Update(deltaTime);
        }
    }

    Scene* SceneManager::GetCurrentScene() noexcept
    {
        return currentScene_.get();
    }

    const Scene* SceneManager::GetCurrentScene() const noexcept
    {
        return currentScene_.get();
    }
} // namespace MyGameEngine
