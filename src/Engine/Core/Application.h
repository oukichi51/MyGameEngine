#pragma once

#include <memory>
#include "Engine/Scene/Scene.h"

struct HINSTANCE__;
using HINSTANCE = HINSTANCE__*;

namespace MyGameEngine
{
    class Engine;

    class Application final
    {
    public:
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        int Run(HINSTANCE instance, int showCommand);

    private:
        std::unique_ptr<Scene> CreateInitialScene();
        std::unique_ptr<Scene> CreateSecondScene();
    private:
        std::unique_ptr<Engine> engine_;
    };
} // namespace MyGameEngine
