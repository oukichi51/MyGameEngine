#include "Game/Scenes/GameScene.h"

#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Scene/Components/MeshRenderer.h"
#include "Engine/Scene/GameObject.h"

#include <memory>

namespace MyGameEngine
{
    GameScene::GameScene()
    {
        CreateObjects();
    }

    void GameScene::CreateObjects()
    {
        auto triangle = Mesh::LoadObj("assets/models/triangle.obj");
        if (triangle == nullptr)
        {
            triangle = Mesh::CreateTriangle();
        }

        const auto quad = Mesh::CreateQuad();

        auto redMaterial = std::make_shared<Material>();
        redMaterial->SetColor(1.0f, 0.35f, 0.25f);

        auto blueMaterial = std::make_shared<Material>();
        blueMaterial->SetColor(0.25f, 0.55f, 1.0f);

        auto& leftObject = CreateGameObject();
        leftObject.GetTransform().SetPosition(-1.15f, 0.0f, 0.0f);
        leftObject.AddComponent<MeshRenderer>(triangle, redMaterial);

        auto& centerObject = CreateGameObject();
        centerObject.GetTransform().SetPosition(0.0f, 0.0f, 0.35f);
        centerObject.AddComponent<MeshRenderer>(quad, blueMaterial);

        auto& rightObject = CreateGameObject();
        rightObject.GetTransform().SetPosition(1.15f, 0.0f, 0.0f);
        rightObject.AddComponent<MeshRenderer>(triangle, redMaterial);
    }
} // namespace MyGameEngine
