#pragma once

#include <Windows.h>

namespace MyGameEngine
{
    class Window final
    {
    public:
        Window() = default;
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool Create(HINSTANCE instance, int showCommand, const wchar_t* title, int clientWidth, int clientHeight);
        HWND GetHandle() const noexcept;

    private:
        static LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

        HINSTANCE instance_ = nullptr;
        HWND handle_ = nullptr;
        const wchar_t* className_ = L"MyGameEngineWindowClass";
    };
} // namespace MyGameEngine
