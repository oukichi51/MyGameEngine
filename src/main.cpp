#include "Engine/Core/Application.h"

#include <Windows.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand)
{
    MyGameEngine::Application application;
    return application.Run(instance, showCommand);
}
