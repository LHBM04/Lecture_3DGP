#include "Precompiled.h"

#include "Mesh.h"

#include "RenderSystem.h"

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

	const std::streampos beginPosition{ file.tellg() };
	const std::string firstTag{ ReadTag(file) };
	if (firstTag == "<Mesh>:" || firstTag == "<SkinnedMesh>:")
	{
		if (!ReadValue(file, sourceVertexCount))
		{
			return false;
		}

		meshName = ReadString(file);
	}
	else
	{
		file.clear();
		file.seekg(beginPosition);
	}

	while (file.peek() != EOF)
	{
		const std::string tag{ ReadTag(file) };
		if (tag.empty())
		{
			break;
		}

		if (tag == "<Bounds>:")
		{
			ReadVector3(file, boundsCenter);
			ReadVector3(file, boundsExtents);
			UpdateBoundsFromCenterExtents();
		}
		else if (tag == "<BoundingBox>:")
		{
			ReadVector3(file, boundsMin);
			ReadVector3(file, boundsMax);
			UpdateBoundsFromMinMax();
		}
		else if (tag == "<Positions>:")
		{
			uint32_t count{ 0 };
			if (!ReadValue(file, count)) return false;

			positions.resize(count);
			for (Vector3D& position : positions)
			{
				ReadVector3(file, position);
			}
		}
		else if (tag == "<Colors>:")
		{
			uint32_t count{ 0 };
			if (!ReadValue(file, count)) return false;

			colors.resize(count);
			for (ColorRGBA& color : colors)
			{
				ReadColor(file, color);
			}
		}
		else if (tag == "<Normals>:")
		{
			uint32_t count{ 0 };
			if (!ReadValue(file, count)) return false;

			normals.resize(count);
			for (Vector3D& normal : normals)
			{
				ReadVector3(file, normal);
			}
		}
		else if (tag == "<TextureCoords>:")
		{
			uint32_t count{ 0 };
			if (!ReadValue(file, count)) return false;

			textureCoordinates.resize(count);
			for (Vector2D& uv : textureCoordinates)
			{
				ReadVector2(file, uv);
			}
		}
		else if (tag == "<SubMeshes>:")
		{
			uint32_t subMeshCount{ 0 };
			if (!ReadValue(file, subMeshCount)) return false;

			subMeshes.clear();
			subMeshes.resize(subMeshCount);
		}
		else if (tag == "<SubMesh>:")
		{
			uint32_t subMeshIndex{ 0 };
			uint32_t indexCount{ 0 };
			if (!ReadValue(file, subMeshIndex)) return false;
			if (!ReadValue(file, indexCount)) return false;

			if (subMeshes.size() <= subMeshIndex)
			{
				subMeshes.resize(subMeshIndex + 1);
			}

			std::vector<uint32_t>& subMeshIndices{ subMeshes[subMeshIndex] };
			subMeshIndices.resize(indexCount);
			if (!static_cast<bool>(file.read(reinterpret_cast<char*>(subMeshIndices.data()), sizeof(uint32_t) * indexCount)))
			{
				return false;
			}
		}
		else if (tag == "<Indices>:")
		{
			uint32_t indexCount{ 0 };
			if (!ReadValue(file, indexCount)) return false;

			indices.resize(indexCount);
			if (!static_cast<bool>(file.read(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * indexCount)))
			{
				return false;
			}
		}
		else if (tag == "</Mesh>")
		{
			break;
		}
		else
		{
			break;
		}
	}

	if (sourceVertexCount == 0)
	{
		sourceVertexCount = static_cast<uint32_t>(positions.size());
	}

	RebuildVertices();
	if (indices.empty())
	{
		RebuildFlattenedIndices();
	}

	ID3D12Device* device{ RenderSystem::GetInstance().GetDevice() };
	if (device == nullptr)
	{
		return true;
	}

	return CreateBuffers(device);
}

void Mesh::Unload()
{
	meshName.clear();
	sourceVertexCount = 0;

	vertices.clear();
	indices.clear();
	positions.clear();
	normals.clear();
	textureCoordinates.clear();
	colors.clear();
	subMeshes.clear();

	vertexBuffer.Reset();
	indexBuffer.Reset();
	vertexBufferView = {};
	indexBufferView = {};

	boundsCenter = {};
	boundsExtents = {};
	boundsMin = {};
	boundsMax = {};
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

const std::vector<ColorRGBA>& Mesh::GetColors() const noexcept
{
	return colors;
}

const std::vector<Vector2D>& Mesh::GetTextureCoordinates() const noexcept
{
	return textureCoordinates;
}

const std::vector<std::vector<uint32_t>>& Mesh::GetSubMeshes() const noexcept
{
	return subMeshes;
}

const std::wstring& Mesh::GetMeshName() const noexcept
{
	return meshName;
}

uint32_t Mesh::GetSourceVertexCount() const noexcept
{
	return sourceVertexCount;
}

const Vector3D& Mesh::GetBoundsCenter() const noexcept
{
	return boundsCenter;
}

const Vector3D& Mesh::GetBoundsExtents() const noexcept
{
	return boundsExtents;
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

bool Mesh::HasGpuBuffers() const noexcept
{
	return vertexBuffer != nullptr;
}

bool Mesh::CreateBuffers(ID3D12Device* device_)
{
	if (device_ == nullptr)
	{
		return false;
	}

	if (HasGpuBuffers())
	{
		return true;
	}

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

void Mesh::ReadVector2(std::ifstream& file_, Vector2D& vector_)
{
	ReadValue(file_, vector_.x);
	ReadValue(file_, vector_.y);
}

void Mesh::ReadVector3(std::ifstream& file_, Vector3D& vector_)
{
	ReadValue(file_, vector_.x);
	ReadValue(file_, vector_.y);
	ReadValue(file_, vector_.z);
}

void Mesh::ReadColor(std::ifstream& file_, ColorRGBA& color_)
{
	ReadValue(file_, color_.x);
	ReadValue(file_, color_.y);
	ReadValue(file_, color_.z);
	ReadValue(file_, color_.w);
}

void Mesh::UpdateBoundsFromCenterExtents() noexcept
{
	boundsMin = boundsCenter - boundsExtents;
	boundsMax = boundsCenter + boundsExtents;
}

void Mesh::UpdateBoundsFromMinMax() noexcept
{
	boundsCenter = (boundsMin + boundsMax) * 0.5f;
	boundsExtents = (boundsMax - boundsMin) * 0.5f;
}

void Mesh::RebuildVertices()
{
	vertices.resize(positions.size());
	for (std::size_t i{ 0 }; i < positions.size(); ++i)
	{
		vertices[i].position = positions[i];
		vertices[i].normal = (i < normals.size()) ? normals[i] : Vector3D::GetUp();
	}
}

void Mesh::RebuildFlattenedIndices()
{
	if (subMeshes.empty())
	{
		return;
	}

	indices.clear();
	for (const std::vector<uint32_t>& subMeshIndices : subMeshes)
	{
		indices.insert(indices.end(), subMeshIndices.begin(), subMeshIndices.end());
	}
}
