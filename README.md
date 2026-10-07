# MyGameEngine

MyGameEngine is a custom game engine built with C++ and DirectX 12 for Windows.

This repository starts as a new project from Phase 6. It does not reuse the previous Renderer project; only the learned DirectX 12 concepts will inform later implementation.

The project is currently at the early engine-foundation stage. It provides a Visual Studio C++20 project, a minimal application/engine lifecycle, a Win32 window with a message loop, keyboard input and frame-time tracking, a Scene that owns and updates GameObjects, and a minimal texture ResourceManager. Each GameObject owns a Transform and its Components. The rendering foundation creates a DXGI factory, selects a high-performance adapter, and initializes a Direct3D 12 device with a WARP fallback. It also owns a flip-model swap chain, double-buffered render targets, and their RTV descriptor heap.

Planned systems include GameObject, Component, Scene, Resource Manager, Input, and Time. These will be introduced incrementally from Day 41 onward.

## Requirements

- Windows 10 or later
- Visual Studio 2022 with the **Desktop development with C++** workload
- Windows 10/11 SDK

## Build

Open `MyGameEngine.sln`, select `Debug` or `Release` and `x64`, then build and run `MyGameEngine`.

## Current structure

- `src/Engine/Core`: application and engine lifecycle, including frame-time tracking
- `src/Engine/Input`: keyboard input state tracking
- `src/Engine/Platform`: Win32 platform integration
- `src/Engine/Resource`: resource ownership and caching, currently starting with textures
- `src/Engine/Graphics`: DirectX 12 renderer, camera, meshes, materials, render queue, and GPU synchronization
- `src/Engine/Scene`: Scene, GameObject, Transform, and reusable Component types
- `assets`: shaders, textures, and models
