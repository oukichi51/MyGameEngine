#include "Engine/Resource/ResourceManager.h"

#include "Engine/Resource/Texture.h"

#include <utility>

namespace MyGameEngine
{
    std::shared_ptr<Texture> ResourceManager::LoadTexture(const std::filesystem::path& filePath)
    {
        if (filePath.empty())
        {
            return nullptr;
        }

        const std::wstring key = MakeTextureKey(filePath);
        if (const auto iterator = textures_.find(key); iterator != textures_.end())
        {
            return iterator->second;
        }

        auto texture = std::make_shared<Texture>(std::filesystem::path(key));
        textures_.emplace(key, texture);
        return texture;
    }

    std::shared_ptr<Texture> ResourceManager::GetTexture(const std::filesystem::path& filePath) const
    {
        const auto iterator = textures_.find(MakeTextureKey(filePath));
        return iterator != textures_.end() ? iterator->second : nullptr;
    }

    bool ResourceManager::UnloadTexture(const std::filesystem::path& filePath)
    {
        return textures_.erase(MakeTextureKey(filePath)) > 0;
    }

    void ResourceManager::Clear() noexcept
    {
        textures_.clear();
    }

    std::size_t ResourceManager::GetTextureCount() const noexcept
    {
        return textures_.size();
    }

    std::wstring ResourceManager::MakeTextureKey(const std::filesystem::path& filePath)
    {
        return filePath.lexically_normal().generic_wstring();
    }
} // namespace MyGameEngine
