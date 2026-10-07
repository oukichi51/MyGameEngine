#include "Engine/Graphics/Material.h"

namespace MyGameEngine
{
    void Material::SetColor(float red, float green, float blue, float alpha) noexcept
    {
        color_ = {red, green, blue, alpha};
    }
    const DirectX::XMFLOAT4& Material::GetColor() const noexcept { return color_; }
    void Material::SetTexture(std::shared_ptr<Texture> texture) noexcept { texture_ = std::move(texture); }
    const std::shared_ptr<Texture>& Material::GetTexture() const noexcept { return texture_; }
} // namespace MyGameEngine
