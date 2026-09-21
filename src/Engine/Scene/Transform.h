#pragma once

#include <DirectXMath.h>

namespace MyGameEngine
{
    class Transform final
    {
    public:
        const DirectX::XMFLOAT3& GetPosition() const noexcept;
        const DirectX::XMFLOAT3& GetRotation() const noexcept;
        const DirectX::XMFLOAT3& GetScale() const noexcept;

        void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
        void SetPosition(float x, float y, float z) noexcept;
        void Translate(const DirectX::XMFLOAT3& translation) noexcept;

        void SetRotation(const DirectX::XMFLOAT3& rotation) noexcept;
        void SetRotation(float pitch, float yaw, float roll) noexcept;
        void Rotate(const DirectX::XMFLOAT3& rotation) noexcept;

        void SetScale(const DirectX::XMFLOAT3& scale) noexcept;
        void SetScale(float x, float y, float z) noexcept;

        DirectX::XMMATRIX GetWorldMatrix() const noexcept;

    private:
        DirectX::XMFLOAT3 position_{0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT3 rotation_{0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT3 scale_{1.0f, 1.0f, 1.0f};
    };
} // namespace MyGameEngine
