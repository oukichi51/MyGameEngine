#include "Engine/Core/Application.h"

#include "Engine/Core/Engine.h"
#include "Game/Scenes/GameScene.h"

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

        engine_->SetScene(std::make_unique<GameScene>());

        const int exitCode = engine_->Run();
        engine_->Shutdown();
        engine_.reset();
        return exitCode;
    }

} // namespace MyGameEngine
