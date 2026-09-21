#include "Engine/Scene/Transform.h"

namespace MyGameEngine
{
    const DirectX::XMFLOAT3& Transform::GetPosition() const noexcept
    {
        return position_;
    }

    const DirectX::XMFLOAT3& Transform::GetRotation() const noexcept
    {
        return rotation_;
    }

    const DirectX::XMFLOAT3& Transform::GetScale() const noexcept
    {
        return scale_;
    }

    void Transform::SetPosition(const DirectX::XMFLOAT3& position) noexcept
    {
        position_ = position;
    }

    void Transform::SetPosition(float x, float y, float z) noexcept
    {
        position_ = {x, y, z};
    }

    void Transform::Translate(const DirectX::XMFLOAT3& translation) noexcept
    {
        position_.x += translation.x;
        position_.y += translation.y;
        position_.z += translation.z;
    }

    void Transform::SetRotation(const DirectX::XMFLOAT3& rotation) noexcept
    {
        rotation_ = rotation;
    }

    void Transform::SetRotation(float pitch, float yaw, float roll) noexcept
    {
        rotation_ = {pitch, yaw, roll};
    }

    void Transform::Rotate(const DirectX::XMFLOAT3& rotation) noexcept
    {
        rotation_.x += rotation.x;
        rotation_.y += rotation.y;
        rotation_.z += rotation.z;
    }

    void Transform::SetScale(const DirectX::XMFLOAT3& scale) noexcept
    {
        scale_ = scale;
    }

    void Transform::SetScale(float x, float y, float z) noexcept
    {
        scale_ = {x, y, z};
    }

    DirectX::XMMATRIX Transform::GetWorldMatrix() const noexcept
    {
        const DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
        const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);
        const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);
        // DirectXMath は行ベクトルを使用するため、ローカル変換は左から右の順に適用
        return scale * rotation * translation;
    }
} // namespace MyGameEngine
