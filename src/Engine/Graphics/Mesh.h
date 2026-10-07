#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

namespace MyGameEngine
{
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 textureCoordinate;
    };

    class Mesh final
    {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<std::uint32_t> indices);

        static std::shared_ptr<Mesh> CreateTriangle();
        static std::shared_ptr<Mesh> CreateQuad();
        static std::shared_ptr<Mesh> LoadObj(const std::filesystem::path& filePath);

        bool EnsureUploaded(ID3D12Device* device);
        const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const noexcept;
        const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const noexcept;
        std::uint32_t GetIndexCount() const noexcept;

    private:
        std::vector<Vertex> vertices_;
        std::vector<std::uint32_t> indices_;
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
        Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
        D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
    };
} // namespace MyGameEngine
