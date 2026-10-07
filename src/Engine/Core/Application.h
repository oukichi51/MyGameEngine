#pragma once

#include <memory>
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
        std::unique_ptr<Engine> engine_;
    };
} // namespace MyGameEngine
