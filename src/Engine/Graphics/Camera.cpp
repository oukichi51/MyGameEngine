#include "Engine/Graphics/Camera.h"

namespace MyGameEngine
{
    void Camera::SetPosition(float x, float y, float z) noexcept { position_ = {x, y, z}; }
    void Camera::LookAt(float x, float y, float z) noexcept { target_ = {x, y, z}; }

    void Camera::SetPerspective(float fieldOfViewRadians, float aspectRatio, float nearPlane,
                                float farPlane) noexcept
    {
        fieldOfView_ = fieldOfViewRadians;
        aspectRatio_ = aspectRatio;
        nearPlane_ = nearPlane;
        farPlane_ = farPlane;
    }

    DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
    {
        return DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&position_), DirectX::XMLoadFloat3(&target_),
                                        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

    DirectX::XMMATRIX Camera::GetProjectionMatrix() const noexcept
    {
        return DirectX::XMMatrixPerspectiveFovLH(fieldOfView_, aspectRatio_, nearPlane_, farPlane_);
    }

    const DirectX::XMFLOAT3& Camera::GetPosition() const noexcept { return position_; }
} // namespace MyGameEngine
