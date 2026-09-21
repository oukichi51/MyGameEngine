#pragma once

#include <memory>

struct HINSTANCE__;
using HINSTANCE = HINSTANCE__*;

namespace MyGameEngine
{
    class InputSystem;
    class TimeSystem;
    class Scene;
    class SceneManager;
    class ResourceManager;
    class Window;

    class Engine final
    {
    public:
        Engine();
        ~Engine();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        bool Initialize(HINSTANCE instance, int showCommand);
        int Run();
        void Shutdown();

        InputSystem& GetInputSystem() noexcept;
        const InputSystem& GetInputSystem() const noexcept;
        TimeSystem& GetTimeSystem() noexcept;
        const TimeSystem& GetTimeSystem() const noexcept;
        SceneManager& GetSceneManager() noexcept;
        const SceneManager& GetSceneManager() const noexcept;
        ResourceManager& GetResourceManager() noexcept;
        const ResourceManager& GetResourceManager() const noexcept;

        void SetScene(std::unique_ptr<Scene> scene);

    private:
        void Update();
    private:
        std::unique_ptr<InputSystem> inputSystem_;
        std::unique_ptr<TimeSystem> timeSystem_;
        std::unique_ptr<SceneManager> sceneManager_;
        std::unique_ptr<ResourceManager> resourceManager_;
        std::unique_ptr<Window> window_;
    };
} // namespace MyGameEngine
