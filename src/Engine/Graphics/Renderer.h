#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <array>
#include <cstdint>
#include <vector>

namespace MyGameEngine
{
    class Camera;
    class RenderQueue;

    class Renderer final
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        static constexpr std::uint32_t BackBufferCount = 2;

        bool Initialize(HWND window, std::uint32_t width, std::uint32_t height);
        void Shutdown() noexcept;
        bool Present(bool verticalSync = true);
        bool Render(const RenderQueue& renderQueue, const Camera& camera);
        void WaitForGpu() noexcept;

        ID3D12Device* GetDevice() const noexcept;
        IDXGIFactory6* GetFactory() const noexcept;
        IDXGIAdapter1* GetAdapter() const noexcept;
        ID3D12CommandQueue* GetCommandQueue() const noexcept;
        IDXGISwapChain3* GetSwapChain() const noexcept;
        ID3D12Resource* GetCurrentBackBuffer() const noexcept;
        D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const noexcept;
        std::uint32_t GetCurrentBackBufferIndex() const noexcept;
        DXGI_FORMAT GetBackBufferFormat() const noexcept;

    private:
        bool CreateFactory();
        bool SelectAdapter();
        bool CreateDevice();
        bool CreateCommandQueue();
        bool CreateSwapChain(HWND window, std::uint32_t width, std::uint32_t height);
        bool CreateRenderTargetViews();
        bool CreateCommandObjects();
        bool CreateFence();
        bool CreatePipeline();
        bool CreateDefaultTexture();
        bool BeginFrame();
        bool EndFrame();

#if defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController_;
#endif
        Microsoft::WRL::ComPtr<IDXGIFactory6> factory_;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter_;
        Microsoft::WRL::ComPtr<ID3D12Device> device_;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetViewHeap_;
        std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, BackBufferCount> backBuffers_;
        std::uint32_t renderTargetViewDescriptorSize_ = 0;
        std::uint32_t currentBackBufferIndex_ = 0;
        DXGI_FORMAT backBufferFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
        HANDLE fenceEvent_ = nullptr;
        std::uint64_t fenceValue_ = 0;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
        D3D12_VIEWPORT viewport_{};
        D3D12_RECT scissorRectangle_{};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> shaderResourceHeap_;
        Microsoft::WRL::ComPtr<ID3D12Resource> defaultTexture_;
        Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadBuffer_;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> frameConstantBuffers_;
    };
} // namespace MyGameEngine
