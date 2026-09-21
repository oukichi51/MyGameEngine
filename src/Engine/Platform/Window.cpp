#include "Engine/Platform/Window.h"

namespace MyGameEngine
{
    Window::~Window()
    {
        if (handle_ != nullptr)
        {
            DestroyWindow(handle_);
            handle_ = nullptr;
        }

        if (instance_ != nullptr)
        {
            UnregisterClass(className_, instance_);
        }
    }

    bool Window::Create(HINSTANCE instance, int showCommand, const wchar_t* title, int clientWidth, int clientHeight)
    {
        instance_ = instance;

        WNDCLASSEX windowClass{};
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WindowProcedure;
        windowClass.hInstance = instance_;
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        windowClass.lpszClassName = className_;

        if (RegisterClassEx(&windowClass) == 0)
        {
            return false;
        }

        RECT windowRectangle{0, 0, clientWidth, clientHeight};
        if (!AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE))
        {
            return false;
        }

        handle_ = CreateWindowEx(0, className_, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                 windowRectangle.right - windowRectangle.left,
                                 windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, instance_, nullptr);

        if (handle_ == nullptr)
        {
            return false;
        }

        ShowWindow(handle_, showCommand);
        UpdateWindow(handle_);
        return true;
    }

    HWND Window::GetHandle() const noexcept
    {
        return handle_;
    }

    LRESULT CALLBACK Window::WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(window, message, wParam, lParam);
        }
    }
} // namespace MyGameEngine
