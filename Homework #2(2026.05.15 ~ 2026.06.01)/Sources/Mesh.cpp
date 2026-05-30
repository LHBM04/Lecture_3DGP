#include "Precompiled.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include <fstream>
#include <functional>
#include <string>

Mesh::~Mesh()
{
	Unload();
}

bool Mesh::Load()
{
	if (path.empty())
	{
		return false;
	}

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	if (!ReadTag(file, "<BoundingBox>:")) return false;
	ReadVector3(file, boundsMin);
	ReadVector3(file, boundsMax);

	if (!ReadTag(file, "<Positions>:")) return false;
	uint32_t positionCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&positionCount), sizeof(uint32_t)))) return false;
	
	std::vector<Vector3D> positions(positionCount);
	for (uint32_t i{ 0 }; i < positionCount; ++i)
	{
		ReadVector3(file, positions[i]);
	}

	if (!ReadTag(file, "<Normals>:")) return false;
	uint32_t normalCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&normalCount), sizeof(uint32_t)))) return false;
	
	std::vector<Vector3D> normals(normalCount);
	for (uint32_t i{ 0 }; i < normalCount; ++i)
	{
		ReadVector3(file, normals[i]);
	}

	if (!ReadTag(file, "<TextureCoords>:")) return false;
	uint32_t texCoordCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&texCoordCount), sizeof(uint32_t)))) return false;
	file.seekg(sizeof(float) * 2 * texCoordCount, std::ios::cur);

	if (!ReadTag(file, "<Indices>:")) return false;
	uint32_t indexCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&indexCount), sizeof(uint32_t)))) return false;
	
	indices.resize(indexCount);
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * indexCount))) return false;

	uint32_t vertexCount = positionCount;
	vertices.resize(vertexCount);
	for (uint32_t i = 0; i < vertexCount; ++i)
	{
		vertices[i].position = positions[i];
		vertices[i].normal = (i < normalCount) ? normals[i] : Vector3D::GetUp();
	}

	return CreateBuffers(RenderSystem::GetInstance().GetDevice());
}

void Mesh::Unload()
{
	vertices.clear();
	indices.clear();
	vertexBuffer.Reset();
	indexBuffer.Reset();
	vertexBufferView = {};
	indexBufferView = {};
}

void Mesh::SetVertices(std::vector<Vertex>&& vertices_)
{
	vertices = std::move(vertices_);
}

void Mesh::SetIndices(std::vector<uint32_t>&& indices_)
{
	indices = std::move(indices_);
}

const std::vector<Vertex>& Mesh::GetVertices() const noexcept
{
	return vertices;
}

const std::vector<uint32_t>& Mesh::GetIndices() const noexcept
{
	return indices;
}

const Vector3D& Mesh::GetBoundsMin() const noexcept
{
	return boundsMin;
}

const Vector3D& Mesh::GetBoundsMax() const noexcept
{
	return boundsMax;
}

ID3D12Resource* Mesh::GetVertexBuffer() const noexcept
{
	return vertexBuffer.Get();
}

const D3D12_VERTEX_BUFFER_VIEW& Mesh::GetVertexBufferView() const noexcept
{
	return vertexBufferView;
}

ID3D12Resource* Mesh::GetIndexBuffer() const noexcept
{
	return indexBuffer.Get();
}

const D3D12_INDEX_BUFFER_VIEW& Mesh::GetIndexBufferView() const noexcept
{
	return indexBufferView;
}

uint32_t Mesh::GetIndexCount() const noexcept
{
	return static_cast<uint32_t>(indices.size());
}

bool Mesh::CreateBuffers(ID3D12Device* device_)
{
	if (vertices.empty())
	{
		return false;
	}

	{
		const UINT bufferSize{ static_cast<UINT>(sizeof(Vertex) * vertices.size()) };
		
		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC bufferDesc{};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = bufferSize;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device_->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer))))
		{
			return false;
		}

		void* mappedData{ nullptr };
		if (FAILED(vertexBuffer->Map(0, nullptr, &mappedData)))
		{
			return false;
		}
		std::memcpy(mappedData, vertices.data(), bufferSize);
		vertexBuffer->Unmap(0, nullptr);

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = bufferSize;
	}

	if (!indices.empty())
	{
		const UINT bufferSize{ static_cast<UINT>(sizeof(uint32_t) * indices.size()) };

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC bufferDesc{};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = bufferSize;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device_->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer))))
		{
			return false;
		}

		void* mappedData{ nullptr };
		if (FAILED(indexBuffer->Map(0, nullptr, &mappedData)))
		{
			return false;
		}
		std::memcpy(mappedData, indices.data(), bufferSize);
		indexBuffer->Unmap(0, nullptr);

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = bufferSize;
	}

	return true;
}

bool Mesh::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t)))) return false;

	std::string tag(tagLength, '\0');
	if (!static_cast<bool>(file_.read(&tag[0], tagLength))) return false;

	return tag == expectedTag_;
}

void Mesh::ReadVector3(std::ifstream& file_, Vector3D& vector_)
{
	file_.read(reinterpret_cast<char*>(&vector_.x), sizeof(float));
	file_.read(reinterpret_cast<char*>(&vector_.y), sizeof(float));
	file_.read(reinterpret_cast<char*>(&vector_.z), sizeof(float));
}
