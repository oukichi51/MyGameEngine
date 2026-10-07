#include "Engine/Scene/Scene.h"

#include "Engine/Scene/GameObject.h"

#include <algorithm>

namespace MyGameEngine
{
    GameObject& Scene::CreateGameObject()
    {
        auto gameObject = std::make_unique<GameObject>();
        GameObject& gameObjectReference = *gameObject;
        gameObjects_.push_back(std::move(gameObject));
        return gameObjectReference;
    }

    bool Scene::DestroyGameObject(const GameObject& gameObject)
    {
        const auto isTargetGameObject = [&gameObject](const std::unique_ptr<GameObject>& candidate) {
            return candidate.get() == &gameObject;
        };

        const auto iterator = std::find_if(gameObjects_.begin(), gameObjects_.end(), isTargetGameObject);
        if (iterator == gameObjects_.end())
        {
            return false;
        }

        gameObjects_.erase(iterator);
        return true;
    }

    void Scene::Update(float deltaTime)
    {
        for (auto& gameObject : gameObjects_)
        {
            gameObject->Update(deltaTime);
        }
    }

    void Scene::Clear() noexcept
    {
        gameObjects_.clear();
    }

    std::size_t Scene::GetGameObjectCount() const noexcept
    {
        return gameObjects_.size();
    }

    const std::vector<std::unique_ptr<GameObject>>& Scene::GetGameObjects() const noexcept
    {
        return gameObjects_;
    }
} // namespace MyGameEngine
