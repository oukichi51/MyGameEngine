#include "Engine/Graphics/Mesh.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace MyGameEngine
{
    namespace
    {
        D3D12_HEAP_PROPERTIES UploadHeapProperties()
        {
            D3D12_HEAP_PROPERTIES properties{};
            properties.Type = D3D12_HEAP_TYPE_UPLOAD;
            properties.CreationNodeMask = 1;
            properties.VisibleNodeMask = 1;
            return properties;
        }

        D3D12_RESOURCE_DESC BufferDescription(UINT64 size)
        {
            D3D12_RESOURCE_DESC description{};
            description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            description.Width = size;
            description.Height = 1;
            description.DepthOrArraySize = 1;
            description.MipLevels = 1;
            description.SampleDesc.Count = 1;
            description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            return description;
        }
    }

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<std::uint32_t> indices)
        : vertices_(std::move(vertices)), indices_(std::move(indices))
    {
    }

    std::shared_ptr<Mesh> Mesh::CreateTriangle()
    {
        return std::make_shared<Mesh>(
            std::vector<Vertex>{{{-0.75f, -0.65f, 0.0f}, {0, 0, -1}, {0, 1}},
                                {{0.0f, 0.75f, 0.0f}, {0, 0, -1}, {0.5f, 0}},
                                {{0.75f, -0.65f, 0.0f}, {0, 0, -1}, {1, 1}}},
            std::vector<std::uint32_t>{0, 1, 2});
    }

    std::shared_ptr<Mesh> Mesh::CreateQuad()
    {
        return std::make_shared<Mesh>(
            std::vector<Vertex>{{{-0.5f, -0.5f, 0}, {0, 0, -1}, {0, 1}},
                                {{-0.5f, 0.5f, 0}, {0, 0, -1}, {0, 0}},
                                {{0.5f, 0.5f, 0}, {0, 0, -1}, {1, 0}},
                                {{0.5f, -0.5f, 0}, {0, 0, -1}, {1, 1}}},
            std::vector<std::uint32_t>{0, 1, 2, 0, 2, 3});
    }

    std::shared_ptr<Mesh> Mesh::LoadObj(const std::filesystem::path& filePath)
    {
        std::ifstream stream(filePath);
        if (!stream)
        {
            return nullptr;
        }

        std::vector<DirectX::XMFLOAT3> positions;
        std::vector<DirectX::XMFLOAT3> normals;
        std::vector<DirectX::XMFLOAT2> textureCoordinates;
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
        std::unordered_map<std::string, std::uint32_t> vertexMap;
        std::string line;

        auto getVertex = [&](const std::string& token) -> std::uint32_t {
            if (const auto found = vertexMap.find(token); found != vertexMap.end()) return found->second;
            std::istringstream tokenStream(token);
            std::string value;
            int positionIndex = 0, textureIndex = 0, normalIndex = 0;
            std::getline(tokenStream, value, '/'); positionIndex = value.empty() ? 0 : std::stoi(value);
            std::getline(tokenStream, value, '/'); textureIndex = value.empty() ? 0 : std::stoi(value);
            std::getline(tokenStream, value, '/'); normalIndex = value.empty() ? 0 : std::stoi(value);
            Vertex vertex{};
            if (positionIndex > 0 && static_cast<std::size_t>(positionIndex) <= positions.size()) vertex.position = positions[positionIndex - 1];
            if (normalIndex > 0 && static_cast<std::size_t>(normalIndex) <= normals.size()) vertex.normal = normals[normalIndex - 1];
            else vertex.normal = {0, 0, -1};
            if (textureIndex > 0 && static_cast<std::size_t>(textureIndex) <= textureCoordinates.size()) vertex.textureCoordinate = textureCoordinates[textureIndex - 1];
            const auto index = static_cast<std::uint32_t>(vertices.size());
            vertices.push_back(vertex);
            vertexMap.emplace(token, index);
            return index;
        };

        while (std::getline(stream, line))
        {
            std::istringstream lineStream(line);
            std::string type;
            lineStream >> type;
            if (type == "v") { DirectX::XMFLOAT3 v{}; lineStream >> v.x >> v.y >> v.z; positions.push_back(v); }
            else if (type == "vn") { DirectX::XMFLOAT3 n{}; lineStream >> n.x >> n.y >> n.z; normals.push_back(n); }
            else if (type == "vt") { DirectX::XMFLOAT2 uv{}; lineStream >> uv.x >> uv.y; uv.y = 1.0f - uv.y; textureCoordinates.push_back(uv); }
            else if (type == "f")
            {
                std::vector<std::uint32_t> face;
                std::string token;
                while (lineStream >> token) face.push_back(getVertex(token));
                for (std::size_t i = 1; i + 1 < face.size(); ++i)
                {
                    indices.push_back(face[0]); indices.push_back(face[i]); indices.push_back(face[i + 1]);
                }
            }
        }
        return vertices.empty() || indices.empty() ? nullptr : std::make_shared<Mesh>(std::move(vertices), std::move(indices));
    }

    bool Mesh::EnsureUploaded(ID3D12Device* device)
    {
        if (vertexBuffer_ && indexBuffer_) return true;
        if (!device || vertices_.empty() || indices_.empty()) return false;
        const UINT64 vertexSize = vertices_.size() * sizeof(Vertex);
        const UINT64 indexSize = indices_.size() * sizeof(std::uint32_t);
        const auto heap = UploadHeapProperties();
        auto vertexDescription = BufferDescription(vertexSize);
        auto indexDescription = BufferDescription(indexSize);
        if (FAILED(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &vertexDescription,
                                                   D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                   IID_PPV_ARGS(&vertexBuffer_))) ||
            FAILED(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &indexDescription,
                                                   D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                   IID_PPV_ARGS(&indexBuffer_)))) return false;
        void* mapped = nullptr;
        if (FAILED(vertexBuffer_->Map(0, nullptr, &mapped))) return false;
        std::memcpy(mapped, vertices_.data(), static_cast<std::size_t>(vertexSize));
        vertexBuffer_->Unmap(0, nullptr);
        if (FAILED(indexBuffer_->Map(0, nullptr, &mapped))) return false;
        std::memcpy(mapped, indices_.data(), static_cast<std::size_t>(indexSize));
        indexBuffer_->Unmap(0, nullptr);
        vertexBufferView_ = {vertexBuffer_->GetGPUVirtualAddress(), static_cast<UINT>(vertexSize), sizeof(Vertex)};
        indexBufferView_ = {indexBuffer_->GetGPUVirtualAddress(), static_cast<UINT>(indexSize), DXGI_FORMAT_R32_UINT};
        return true;
    }

    const D3D12_VERTEX_BUFFER_VIEW& Mesh::GetVertexBufferView() const noexcept { return vertexBufferView_; }
    const D3D12_INDEX_BUFFER_VIEW& Mesh::GetIndexBufferView() const noexcept { return indexBufferView_; }
    std::uint32_t Mesh::GetIndexCount() const noexcept { return static_cast<std::uint32_t>(indices_.size()); }
} // namespace MyGameEngine
