#include "Engine/Core/Application.h"

#include "Engine/Core/Engine.h"

#include <Windows.h>

namespace MyGameEngine
{
    Application::Application() = default;
    Application::~Application() = default;

    int Application::Run(HINSTANCE instance, int showCommand)
    {
        engine_ = std::make_unique<Engine>();
        if (!engine_->Initialize(instance, showCommand))
        {
            engine_.reset();
            return 1;
        }

        engine_->SetScene(CreateInitialScene());
        engine_->SetScene(CreateSecondScene());

        const int exitCode = engine_->Run();
        engine_->Shutdown();
        engine_.reset();
        return exitCode;
    }

    std::unique_ptr<MyGameEngine::Scene> Application::CreateInitialScene()
    {
        auto scene = std::make_unique<MyGameEngine::Scene>();

        auto& gameObject = scene->CreateGameObject();
        gameObject.AddComponent<MyGameEngine::MoveComponent>();

        MoveComponent* moveComponent = gameObject.GetComponent<MoveComponent>();
        if (moveComponent != nullptr)
        {
            OutputDebugStringA("MoveComponent found\n");
        }

        return scene;
    }

    std::unique_ptr<Scene> Application::CreateSecondScene()
    {
        auto scene = std::make_unique<Scene>();

        auto& gameObject = scene->CreateGameObject();
        gameObject.AddComponent<MoveComponent>();

        return scene;
    }
} // namespace MyGameEngine
