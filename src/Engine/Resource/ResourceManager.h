#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "Engine/Resource/Texture.h"

namespace MyGameEngine
{
    class ResourceManager final
    {
    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) noexcept = default;
        ResourceManager& operator=(ResourceManager&&) noexcept = default;

        std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& filePath);
        std::shared_ptr<Texture> GetTexture(const std::filesystem::path& filePath) const;
        bool UnloadTexture(const std::filesystem::path& filePath);

        void Clear() noexcept;

        [[nodiscard]] std::size_t GetTextureCount() const noexcept;

    private:
        static std::wstring MakeTextureKey(const std::filesystem::path& filePath);

        std::unordered_map<std::wstring, std::shared_ptr<Texture>> textures_;
    };
} // namespace MyGameEngine
