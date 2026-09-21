#include "Engine/Resource/Texture.h"

#include <utility>

namespace MyGameEngine
{
    Texture::Texture(std::filesystem::path filePath) : filePath_(std::move(filePath))
    {
    }

    const std::filesystem::path& Texture::GetFilePath() const noexcept
    {
        return filePath_;
    }
} // namespace MyGameEngine
