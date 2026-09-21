#pragma once

#include <filesystem>

namespace MyGameEngine
{
    class Texture final
    {
    public:
        explicit Texture(std::filesystem::path filePath);

        const std::filesystem::path& GetFilePath() const noexcept;

    private:
        std::filesystem::path filePath_;
    };
} // namespace MyGameEngine
