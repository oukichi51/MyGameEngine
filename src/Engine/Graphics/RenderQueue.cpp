#include "Engine/Graphics/RenderQueue.h"
#include "Engine/Scene/Components/MeshRenderer.h"
#include "Engine/Scene/GameObject.h"
#include "Engine/Scene/Scene.h"

namespace MyGameEngine
{
    void RenderQueue::Collect(const Scene& scene)
    {
        Clear();
        for (const auto& gameObject : scene.GetGameObjects())
        {
            if (const auto* renderer = gameObject->GetComponent<MeshRenderer>())
            {
                items_.push_back(renderer);
            }
        }
    }
    void RenderQueue::Clear() noexcept { items_.clear(); }
    const std::vector<const MeshRenderer*>& RenderQueue::GetItems() const noexcept { return items_; }
} // namespace MyGameEngine
