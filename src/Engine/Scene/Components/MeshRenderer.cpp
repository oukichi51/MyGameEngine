#include "Engine/Scene/Components/MeshRenderer.h"

namespace MyGameEngine
{
    MeshRenderer::MeshRenderer(GameObject& owner, std::shared_ptr<Mesh> mesh,
                               std::shared_ptr<Material> material) noexcept
        : owner_(owner), mesh_(std::move(mesh)), material_(std::move(material))
    {
    }
    GameObject& MeshRenderer::GetOwner() const noexcept { return owner_; }
    const std::shared_ptr<Mesh>& MeshRenderer::GetMesh() const noexcept { return mesh_; }
    const std::shared_ptr<Material>& MeshRenderer::GetMaterial() const noexcept { return material_; }
} // namespace MyGameEngine
