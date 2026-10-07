#pragma once

#include <DirectXMath.h>

namespace MyGameEngine
{
    class Camera final
    {
    public:
        void SetPosition(float x, float y, float z) noexcept;
        void LookAt(float x, float y, float z) noexcept;
        void SetPerspective(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane) noexcept;

        DirectX::XMMATRIX GetViewMatrix() const noexcept;
        DirectX::XMMATRIX GetProjectionMatrix() const noexcept;
        const DirectX::XMFLOAT3& GetPosition() const noexcept;

    private:
        DirectX::XMFLOAT3 position_{0.0f, 0.0f, -5.0f};
        DirectX::XMFLOAT3 target_{0.0f, 0.0f, 0.0f};
        float fieldOfView_ = DirectX::XM_PIDIV4;
        float aspectRatio_ = 16.0f / 9.0f;
        float nearPlane_ = 0.1f;
        float farPlane_ = 1000.0f;
    };
} // namespace MyGameEngine
