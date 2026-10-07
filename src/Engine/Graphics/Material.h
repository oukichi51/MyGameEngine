#pragma once

#include <DirectXMath.h>
#include <memory>

namespace MyGameEngine
{
    class Texture;

    class Material final
    {
    public:
        void SetColor(float red, float green, float blue, float alpha = 1.0f) noexcept;
        const DirectX::XMFLOAT4& GetColor() const noexcept;
        void SetTexture(std::shared_ptr<Texture> texture) noexcept;
        const std::shared_ptr<Texture>& GetTexture() const noexcept;

    private:
        DirectX::XMFLOAT4 color_{1.0f, 1.0f, 1.0f, 1.0f};
        std::shared_ptr<Texture> texture_;
    };
} // namespace MyGameEngine
