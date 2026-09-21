#include "Engine/Core/Engine.h"
#include "Engine/Core/TimeSystem.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Platform/Window.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Resource/ResourceManager.h"

#include <Windows.h>

namespace MyGameEngine
{
    Engine::Engine() = default;
    Engine::~Engine() = default;

    bool Engine::Initialize(HINSTANCE instance, int showCommand)
    {
        inputSystem_ = std::make_unique<InputSystem>();
        timeSystem_ = std::make_unique<TimeSystem>();
        sceneManager_ = std::make_unique<SceneManager>();
        resourceManager_ = std::make_unique<ResourceManager>();
        window_ = std::make_unique<Window>();
        if (!window_->Create(instance, showCommand, L"MyGameEngine", 1280, 720))
        {
            Shutdown();
            return false;
        }

        return true;
    }

    int Engine::Run()
    {
        MSG message{};
        timeSystem_->Reset();

        while (true)
        {
            while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    return static_cast<int>(message.wParam);
                }

                TranslateMessage(&message);
                DispatchMessage(&message);
            }

            Update();

            // Avoid busy-waiting until rendering and frame pacing are implemented.
            Sleep(1);
        }
    }

    void Engine::Update()
    {
        timeSystem_->Update();
        inputSystem_->Update();
        sceneManager_->Update(timeSystem_->GetDeltaTime());
    }

    void Engine::Shutdown()
    {
        window_.reset();
        timeSystem_.reset();
        inputSystem_.reset();
        sceneManager_.reset();
        resourceManager_.reset();
    }

    InputSystem& Engine::GetInputSystem() noexcept
    {
        return *inputSystem_;
    }

    const InputSystem& Engine::GetInputSystem() const noexcept
    {
        return *inputSystem_;
    }

    TimeSystem& Engine::GetTimeSystem() noexcept
    {
        return *timeSystem_;
    }

    const TimeSystem& Engine::GetTimeSystem() const noexcept
    {
        return *timeSystem_;
    }

    SceneManager& Engine::GetSceneManager() noexcept
    {
        return *sceneManager_;
    }

    const SceneManager& Engine::GetSceneManager() const noexcept
    {
        return *sceneManager_;
    }

    ResourceManager& Engine::GetResourceManager() noexcept
    {
        return *resourceManager_;
    }

    const ResourceManager& Engine::GetResourceManager() const noexcept
    {
        return *resourceManager_;
    } 

    void Engine::SetScene(std::unique_ptr<Scene> scene)
    {
        sceneManager_->SetScene(std::move(scene));
    }
} // namespace MyGameEngine
