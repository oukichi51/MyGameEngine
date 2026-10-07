#pragma once

#include <filesystem>
#include <cstdint>
#include <vector>

namespace MyGameEngine
{
    class Texture final
    {
    public:
        explicit Texture(std::filesystem::path filePath);

        const std::filesystem::path& GetFilePath() const noexcept;
        bool IsLoaded() const noexcept;
        std::uint32_t GetWidth() const noexcept;
        std::uint32_t GetHeight() const noexcept;
        const std::vector<std::uint8_t>& GetPixels() const noexcept;

    private:
        std::filesystem::path filePath_;
        std::uint32_t width_ = 0;
        std::uint32_t height_ = 0;
        std::vector<std::uint8_t> pixels_;
    };
} // namespace MyGameEngine
