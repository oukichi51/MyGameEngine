#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/Camera.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/RenderQueue.h"
#include "Engine/Scene/Components/MeshRenderer.h"
#include "Engine/Scene/GameObject.h"

#include <d3dcompiler.h>

#include <cstring>

namespace MyGameEngine
{
    bool Renderer::Initialize(HWND window, std::uint32_t width, std::uint32_t height)
    {
        Shutdown();

        if (window == nullptr || width == 0 || height == 0)
        {
            return false;
        }

#if defined(_DEBUG)
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_))))
        {
            debugController_->EnableDebugLayer();
        }
#endif

        if (!CreateFactory() || !SelectAdapter() || !CreateDevice() || !CreateCommandQueue() ||
            !CreateSwapChain(window, width, height) || !CreateRenderTargetViews() ||
            !CreateCommandObjects() || !CreateFence() || !CreateDefaultTexture() || !CreatePipeline())
        {
            Shutdown();
            return false;
        }

        viewport_ = {0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f};
        scissorRectangle_ = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        return true;
    }

    void Renderer::Shutdown() noexcept
    {
        WaitForGpu();
        frameConstantBuffers_.clear();
        textureUploadBuffer_.Reset();
        defaultTexture_.Reset();
        shaderResourceHeap_.Reset();
        pipelineState_.Reset();
        rootSignature_.Reset();
        commandList_.Reset();
        commandAllocator_.Reset();
        fence_.Reset();
        if (fenceEvent_ != nullptr)
        {
            CloseHandle(fenceEvent_);
            fenceEvent_ = nullptr;
        }
        for (auto& backBuffer : backBuffers_)
        {
            backBuffer.Reset();
        }
        renderTargetViewHeap_.Reset();
        swapChain_.Reset();
        commandQueue_.Reset();
        device_.Reset();
        adapter_.Reset();
        factory_.Reset();
#if defined(_DEBUG)
        debugController_.Reset();
#endif
        renderTargetViewDescriptorSize_ = 0;
        currentBackBufferIndex_ = 0;
        fenceValue_ = 0;
    }

    void Renderer::WaitForGpu() noexcept
    {
        if (!commandQueue_ || !fence_ || fenceEvent_ == nullptr) return;
        const std::uint64_t value = ++fenceValue_;
        if (FAILED(commandQueue_->Signal(fence_.Get(), value))) return;
        if (fence_->GetCompletedValue() < value &&
            SUCCEEDED(fence_->SetEventOnCompletion(value, fenceEvent_)))
        {
            WaitForSingleObject(fenceEvent_, INFINITE);
        }
    }

    bool Renderer::Present(bool verticalSync)
    {
        if (swapChain_ == nullptr || FAILED(swapChain_->Present(verticalSync ? 1u : 0u, 0)))
        {
            return false;
        }

        currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
        return true;
    }

    ID3D12Device* Renderer::GetDevice() const noexcept { return device_.Get(); }
    IDXGIFactory6* Renderer::GetFactory() const noexcept { return factory_.Get(); }
    IDXGIAdapter1* Renderer::GetAdapter() const noexcept { return adapter_.Get(); }
    ID3D12CommandQueue* Renderer::GetCommandQueue() const noexcept { return commandQueue_.Get(); }
    IDXGISwapChain3* Renderer::GetSwapChain() const noexcept { return swapChain_.Get(); }
    ID3D12Resource* Renderer::GetCurrentBackBuffer() const noexcept
    {
        return backBuffers_[currentBackBufferIndex_].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetCurrentRenderTargetView() const noexcept
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle{};
        if (renderTargetViewHeap_ != nullptr)
        {
            handle = renderTargetViewHeap_->GetCPUDescriptorHandleForHeapStart();
            handle.ptr += static_cast<SIZE_T>(currentBackBufferIndex_) * renderTargetViewDescriptorSize_;
        }
        return handle;
    }

    std::uint32_t Renderer::GetCurrentBackBufferIndex() const noexcept
    {
        return currentBackBufferIndex_;
    }

    DXGI_FORMAT Renderer::GetBackBufferFormat() const noexcept { return backBufferFormat_; }

    bool Renderer::CreateFactory()
    {
        UINT flags = 0;
#if defined(_DEBUG)
        if (debugController_ != nullptr)
        {
            flags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif
        return SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory_)));
    }

    bool Renderer::SelectAdapter()
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> candidate;
        for (UINT index = 0;
             factory_->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                  IID_PPV_ARGS(&candidate)) != DXGI_ERROR_NOT_FOUND;
             ++index)
        {
            DXGI_ADAPTER_DESC1 description{};
            if (FAILED(candidate->GetDesc1(&description)))
            {
                candidate.Reset();
                continue;
            }

            const bool isSoftwareAdapter = (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0;
            if (!isSoftwareAdapter &&
                SUCCEEDED(D3D12CreateDevice(candidate.Get(), D3D_FEATURE_LEVEL_12_0,
                                            __uuidof(ID3D12Device), nullptr)))
            {
                adapter_ = candidate;
                return true;
            }

            candidate.Reset();
        }

        Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
        if (FAILED(factory_->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
        {
            return false;
        }

        return SUCCEEDED(warpAdapter.As(&adapter_));
    }

    bool Renderer::CreateDevice()
    {
        return SUCCEEDED(D3D12CreateDevice(adapter_.Get(), D3D_FEATURE_LEVEL_12_0,
                                           IID_PPV_ARGS(&device_)));
    }

    bool Renderer::CreateCommandQueue()
    {
        D3D12_COMMAND_QUEUE_DESC description{};
        description.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        description.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        return SUCCEEDED(device_->CreateCommandQueue(&description, IID_PPV_ARGS(&commandQueue_)));
    }

    bool Renderer::CreateSwapChain(HWND window, std::uint32_t width, std::uint32_t height)
    {
        DXGI_SWAP_CHAIN_DESC1 description{};
        description.Width = width;
        description.Height = height;
        description.Format = backBufferFormat_;
        description.SampleDesc.Count = 1;
        description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.BufferCount = BackBufferCount;
        description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
        if (FAILED(factory_->CreateSwapChainForHwnd(commandQueue_.Get(), window, &description, nullptr,
                                                    nullptr, &swapChain)))
        {
            return false;
        }

        if (FAILED(factory_->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER)) ||
            FAILED(swapChain.As(&swapChain_)))
        {
            return false;
        }

        currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
        return true;
    }

    bool Renderer::CreateRenderTargetViews()
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDescription{};
        heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDescription.NumDescriptors = BackBufferCount;
        heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(device_->CreateDescriptorHeap(&heapDescription,
                                                 IID_PPV_ARGS(&renderTargetViewHeap_))))
        {
            return false;
        }

        renderTargetViewDescriptorSize_ =
            device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE handle =
            renderTargetViewHeap_->GetCPUDescriptorHandleForHeapStart();

        for (std::uint32_t index = 0; index < BackBufferCount; ++index)
        {
            if (FAILED(swapChain_->GetBuffer(index, IID_PPV_ARGS(&backBuffers_[index]))))
            {
                return false;
            }

            device_->CreateRenderTargetView(backBuffers_[index].Get(), nullptr, handle);
            handle.ptr += renderTargetViewDescriptorSize_;
        }

        return true;
    }

    bool Renderer::CreateCommandObjects()
    {
        if (FAILED(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                   IID_PPV_ARGS(&commandAllocator_))) ||
            FAILED(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
                                              nullptr, IID_PPV_ARGS(&commandList_))))
        {
            return false;
        }
        return SUCCEEDED(commandList_->Close());
    }

    bool Renderer::CreateFence()
    {
        if (FAILED(device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)))) return false;
        fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        return fenceEvent_ != nullptr;
    }

    bool Renderer::CreateDefaultTexture()
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDescription{};
        heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDescription.NumDescriptors = 1;
        heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (FAILED(device_->CreateDescriptorHeap(&heapDescription, IID_PPV_ARGS(&shaderResourceHeap_)))) return false;

        D3D12_RESOURCE_DESC textureDescription{};
        textureDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        textureDescription.Width = 2;
        textureDescription.Height = 2;
        textureDescription.DepthOrArraySize = 1;
        textureDescription.MipLevels = 1;
        textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDescription.SampleDesc.Count = 1;
        textureDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        D3D12_HEAP_PROPERTIES defaultHeap{};
        defaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;
        defaultHeap.CreationNodeMask = defaultHeap.VisibleNodeMask = 1;
        if (FAILED(device_->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &textureDescription,
                                                   D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                   IID_PPV_ARGS(&defaultTexture_)))) return false;

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
        UINT rows = 0;
        UINT64 rowSize = 0, uploadSize = 0;
        device_->GetCopyableFootprints(&textureDescription, 0, 1, 0, &footprint, &rows, &rowSize, &uploadSize);
        D3D12_RESOURCE_DESC bufferDescription{};
        bufferDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDescription.Width = uploadSize;
        bufferDescription.Height = 1;
        bufferDescription.DepthOrArraySize = 1;
        bufferDescription.MipLevels = 1;
        bufferDescription.SampleDesc.Count = 1;
        bufferDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        D3D12_HEAP_PROPERTIES uploadHeap{};
        uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadHeap.CreationNodeMask = uploadHeap.VisibleNodeMask = 1;
        if (FAILED(device_->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDescription,
                                                   D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                   IID_PPV_ARGS(&textureUploadBuffer_)))) return false;
        constexpr std::uint32_t pixels[4] = {0xffffffff, 0xff404040, 0xff404040, 0xffffffff};
        std::byte* mapped = nullptr;
        if (FAILED(textureUploadBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped)))) return false;
        for (UINT row = 0; row < rows; ++row)
            std::memcpy(mapped + footprint.Offset + row * footprint.Footprint.RowPitch,
                        reinterpret_cast<const std::byte*>(pixels) + row * rowSize, static_cast<std::size_t>(rowSize));
        textureUploadBuffer_->Unmap(0, nullptr);

        if (FAILED(commandAllocator_->Reset()) ||
            FAILED(commandList_->Reset(commandAllocator_.Get(), nullptr))) return false;
        D3D12_TEXTURE_COPY_LOCATION destination{};
        destination.pResource = defaultTexture_.Get();
        destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        D3D12_TEXTURE_COPY_LOCATION source{};
        source.pResource = textureUploadBuffer_.Get();
        source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        source.PlacedFootprint = footprint;
        commandList_->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = defaultTexture_.Get();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        commandList_->ResourceBarrier(1, &barrier);
        if (FAILED(commandList_->Close())) return false;
        ID3D12CommandList* lists[] = {commandList_.Get()};
        commandQueue_->ExecuteCommandLists(1, lists);
        WaitForGpu();

        D3D12_SHADER_RESOURCE_VIEW_DESC view{};
        view.Format = textureDescription.Format;
        view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        view.Texture2D.MipLevels = 1;
        device_->CreateShaderResourceView(defaultTexture_.Get(), &view,
                                          shaderResourceHeap_->GetCPUDescriptorHandleForHeapStart());
        return true;
    }

    bool Renderer::CreatePipeline()
    {
        static constexpr char shaderSource[] = R"(
cbuffer ObjectConstants : register(b0) {
    float4x4 WorldViewProjection; float4x4 World; float4 MaterialColor;
    float3 LightDirection; float Padding;
};
Texture2D DiffuseTexture : register(t0);
SamplerState LinearSampler : register(s0);
struct VSInput { float3 position : POSITION; float3 normal : NORMAL; float2 uv : TEXCOORD; };
struct PSInput { float4 position : SV_POSITION; float3 normal : NORMAL; float2 uv : TEXCOORD; };
PSInput VSMain(VSInput input) {
    PSInput output; output.position = mul(float4(input.position, 1), WorldViewProjection);
    output.normal = normalize(mul(float4(input.normal, 0), World).xyz); output.uv = input.uv; return output;
}
float4 PSMain(PSInput input) : SV_TARGET {
    float diffuse = saturate(dot(normalize(input.normal), -normalize(LightDirection)));
    float lighting = 0.2 + diffuse * 0.8;
    return DiffuseTexture.Sample(LinearSampler, input.uv) * MaterialColor * lighting;
})";
        UINT compileFlags = 0;
#if defined(_DEBUG)
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        Microsoft::WRL::ComPtr<ID3DBlob> vertexShader, pixelShader, errors;
        if (FAILED(D3DCompile(shaderSource, sizeof(shaderSource), "DefaultShader", nullptr, nullptr,
                             "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errors)) ||
            FAILED(D3DCompile(shaderSource, sizeof(shaderSource), "DefaultShader", nullptr, nullptr,
                             "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errors))) return false;

        D3D12_DESCRIPTOR_RANGE range{};
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range.NumDescriptors = 1;
        range.BaseShaderRegister = 0;
        range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        D3D12_ROOT_PARAMETER parameters[2]{};
        parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        parameters[0].Descriptor.ShaderRegister = 0;
        parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameters[1].DescriptorTable.NumDescriptorRanges = 1;
        parameters[1].DescriptorTable.pDescriptorRanges = &range;
        parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        D3D12_STATIC_SAMPLER_DESC sampler{};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        D3D12_ROOT_SIGNATURE_DESC rootDescription{};
        rootDescription.NumParameters = 2;
        rootDescription.pParameters = parameters;
        rootDescription.NumStaticSamplers = 1;
        rootDescription.pStaticSamplers = &sampler;
        rootDescription.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        if (FAILED(D3D12SerializeRootSignature(&rootDescription, D3D_ROOT_SIGNATURE_VERSION_1,
                                               &signature, &errors)) ||
            FAILED(device_->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                                IID_PPV_ARGS(&rootSignature_)))) return false;

        const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline{};
        pipeline.pRootSignature = rootSignature_.Get();
        pipeline.VS = {vertexShader->GetBufferPointer(), vertexShader->GetBufferSize()};
        pipeline.PS = {pixelShader->GetBufferPointer(), pixelShader->GetBufferSize()};
        pipeline.InputLayout = {inputLayout, _countof(inputLayout)};
        pipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipeline.RTVFormats[0] = backBufferFormat_;
        pipeline.NumRenderTargets = 1;
        pipeline.SampleDesc.Count = 1;
        pipeline.SampleMask = UINT_MAX;
        pipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        pipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        pipeline.RasterizerState.DepthClipEnable = TRUE;
        pipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        pipeline.DepthStencilState.DepthEnable = FALSE;
        pipeline.DepthStencilState.StencilEnable = FALSE;
        return SUCCEEDED(device_->CreateGraphicsPipelineState(&pipeline, IID_PPV_ARGS(&pipelineState_)));
    }

    bool Renderer::BeginFrame()
    {
        frameConstantBuffers_.clear();
        if (FAILED(commandAllocator_->Reset()) ||
            FAILED(commandList_->Reset(commandAllocator_.Get(), pipelineState_.Get()))) return false;
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = GetCurrentBackBuffer();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        commandList_->ResourceBarrier(1, &barrier);
        const auto target = GetCurrentRenderTargetView();
        constexpr float clearColor[] = {0.04f, 0.07f, 0.12f, 1.0f};
        commandList_->ClearRenderTargetView(target, clearColor, 0, nullptr);
        commandList_->OMSetRenderTargets(1, &target, FALSE, nullptr);
        commandList_->RSSetViewports(1, &viewport_);
        commandList_->RSSetScissorRects(1, &scissorRectangle_);
        commandList_->SetGraphicsRootSignature(rootSignature_.Get());
        ID3D12DescriptorHeap* heaps[] = {shaderResourceHeap_.Get()};
        commandList_->SetDescriptorHeaps(1, heaps);
        commandList_->SetGraphicsRootDescriptorTable(1, shaderResourceHeap_->GetGPUDescriptorHandleForHeapStart());
        commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        return true;
    }

    bool Renderer::EndFrame()
    {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = GetCurrentBackBuffer();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        commandList_->ResourceBarrier(1, &barrier);
        if (FAILED(commandList_->Close())) return false;
        ID3D12CommandList* lists[] = {commandList_.Get()};
        commandQueue_->ExecuteCommandLists(1, lists);
        if (!Present(true)) return false;
        WaitForGpu();
        return true;
    }

    bool Renderer::Render(const RenderQueue& renderQueue, const Camera& camera)
    {
        if (!BeginFrame()) return false;
        struct ObjectConstants
        {
            DirectX::XMFLOAT4X4 worldViewProjection;
            DirectX::XMFLOAT4X4 world;
            DirectX::XMFLOAT4 materialColor;
            DirectX::XMFLOAT3 lightDirection;
            float padding;
        };

        for (const MeshRenderer* meshRenderer : renderQueue.GetItems())
        {
            if (!meshRenderer || !meshRenderer->GetMesh() || !meshRenderer->GetMaterial() ||
                !meshRenderer->GetMesh()->EnsureUploaded(device_.Get())) continue;
            D3D12_HEAP_PROPERTIES heap{};
            heap.Type = D3D12_HEAP_TYPE_UPLOAD;
            heap.CreationNodeMask = heap.VisibleNodeMask = 1;
            D3D12_RESOURCE_DESC description{};
            description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            description.Width = (sizeof(ObjectConstants) + 255u) & ~255u;
            description.Height = 1;
            description.DepthOrArraySize = 1;
            description.MipLevels = 1;
            description.SampleDesc.Count = 1;
            description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
            if (FAILED(device_->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &description,
                                                       D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                       IID_PPV_ARGS(&constantBuffer)))) return false;
            const auto world = meshRenderer->GetOwner().GetTransform().GetWorldMatrix();
            const auto wvp = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
            ObjectConstants constants{};
            DirectX::XMStoreFloat4x4(&constants.worldViewProjection, DirectX::XMMatrixTranspose(wvp));
            DirectX::XMStoreFloat4x4(&constants.world, DirectX::XMMatrixTranspose(world));
            constants.materialColor = meshRenderer->GetMaterial()->GetColor();
            constants.lightDirection = {0.3f, -0.7f, 1.0f};
            void* mapped = nullptr;
            if (FAILED(constantBuffer->Map(0, nullptr, &mapped))) return false;
            std::memcpy(mapped, &constants, sizeof(constants));
            constantBuffer->Unmap(0, nullptr);
            commandList_->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());
            const auto& vertexView = meshRenderer->GetMesh()->GetVertexBufferView();
            const auto& indexView = meshRenderer->GetMesh()->GetIndexBufferView();
            commandList_->IASetVertexBuffers(0, 1, &vertexView);
            commandList_->IASetIndexBuffer(&indexView);
            commandList_->DrawIndexedInstanced(meshRenderer->GetMesh()->GetIndexCount(), 1, 0, 0, 0);
            frameConstantBuffers_.push_back(std::move(constantBuffer));
        }
        return EndFrame();
    }
} // namespace MyGameEngine
