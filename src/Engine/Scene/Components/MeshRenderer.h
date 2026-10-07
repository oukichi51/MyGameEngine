#pragma once

#include "Engine/Scene/Component.h"

#include <memory>

namespace MyGameEngine
{
    class GameObject;
    class Material;
    class Mesh;

    class MeshRenderer final : public Component
    {
    public:
        MeshRenderer(GameObject& owner, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) noexcept;

        GameObject& GetOwner() const noexcept;
        const std::shared_ptr<Mesh>& GetMesh() const noexcept;
        const std::shared_ptr<Material>& GetMaterial() const noexcept;

    private:
        GameObject& owner_;
        std::shared_ptr<Mesh> mesh_;
        std::shared_ptr<Material> material_;
    };
} // namespace MyGameEngine
