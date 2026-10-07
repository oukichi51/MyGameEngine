#pragma once

#include <vector>

namespace MyGameEngine
{
    class MeshRenderer;
    class Scene;

    class RenderQueue final
    {
    public:
        void Collect(const Scene& scene);
        void Clear() noexcept;
        const std::vector<const MeshRenderer*>& GetItems() const noexcept;

    private:
        std::vector<const MeshRenderer*> items_;
    };
} // namespace MyGameEngine
