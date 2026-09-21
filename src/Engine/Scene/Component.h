#pragma once

namespace MyGameEngine
{
    class Component
    {
    public:
        virtual ~Component() = default;

        virtual void Update(float deltaTime);

    protected:
        Component() = default;

        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;
        Component(Component&&) = default;
        Component& operator=(Component&&) = default;
    };
} // namespace MyGameEngine
