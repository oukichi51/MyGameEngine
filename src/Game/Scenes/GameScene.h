#pragma once

#include "Engine/Scene/Scene.h"

namespace MyGameEngine
{
    class GameScene final : public Scene
    {
    public:
        GameScene();
        ~GameScene() override = default;

        GameScene(const GameScene&) = delete;
        GameScene& operator=(const GameScene&) = delete;
        GameScene(GameScene&&) = delete;
        GameScene& operator=(GameScene&&) = delete;

    private:
        void CreateObjects();
    };
} // namespace MyGameEngine
