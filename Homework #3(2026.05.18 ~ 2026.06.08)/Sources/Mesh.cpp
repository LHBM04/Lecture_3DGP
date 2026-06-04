#include "Precompiled.h"
#include "Mesh.h"

#include "RenderSystem.h"

Mesh::Mesh(std::wstring_view resourceName_, std::filesystem::path sourcePath_)
	: Resource(resourceName_, std::move(sourcePath_))
{
}

bool Mesh::Load()
{
	if (IsLoaded())
	{
		return true;
	}

	if (GetPath().empty())
	{
		return false;
	}

	std::ifstream file(GetPath(), std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	if (!ReadTag(file, "<BoundingBox>:"))
	{
		return false;
	}

	ReadVector3(file, boundsMin);
	ReadVector3(file, boundsMax);

	if (!ReadTag(file, "<Positions>:"))
	{
		return false;
	}

	uint32_t positionCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&positionCount), sizeof(positionCount))))
	{
		return false;
	}

	std::vector<Vector3D> positions(positionCount);
	for (Vector3D& position : positions)
	{
		ReadVector3(file, position);
	}

	if (!ReadTag(file, "<Normals>:"))
	{
		return false;
	}

	uint32_t normalCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&normalCount), sizeof(normalCount))))
	{
		return false;
	}

	std::vector<Vector3D> normals(normalCount);
	for (Vector3D& normal : normals)
	{
		ReadVector3(file, normal);
	}

	if (!ReadTag(file, "<TextureCoords>:"))
	{
		return false;
	}

	uint32_t texCoordCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&texCoordCount), sizeof(texCoordCount))))
	{
		return false;
	}

	file.seekg(static_cast<std::streamoff>(sizeof(float) * 2ull * texCoordCount), std::ios::cur);

	if (!ReadTag(file, "<Indices>:"))
	{
		return false;
	}

	uint32_t loadedIndexCount{ 0 };
	if (!static_cast<bool>(file.read(reinterpret_cast<char*>(&loadedIndexCount), sizeof(loadedIndexCount))))
	{
		return false;
	}

	std::vector<uint32_t> indices(loadedIndexCount);
	if (loadedIndexCount > 0 &&
		!static_cast<bool>(file.read(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * loadedIndexCount)))
	{
		return false;
	}

	std::vector<Vertex> vertices(positionCount);
	for (uint32_t vertexIndex{ 0 }; vertexIndex < positionCount; ++vertexIndex)
	{
		vertices[vertexIndex].position = positions[vertexIndex];
		vertices[vertexIndex].normal = (vertexIndex < normalCount) ? normals[vertexIndex] : Vector3D::GetUp();
	}

	if (!Create(std::span<const Vertex>(vertices), std::span<const uint32_t>(indices)))
	{
		return false;
	}

	MarkLoaded(true);
	return true;
}

void Mesh::Unload()
{
	Release();
}

bool Mesh::Create(
	std::span<const std::byte> vertexData_,
	UINT vertexStride_,
	UINT vertexCount_,
	std::span<const uint32_t> indices_)
{
	Release();

	if (vertexData_.empty() || vertexStride_ == 0 || vertexCount_ == 0)
	{
		return false;
	}

	cpuVertexData.assign(vertexData_.begin(), vertexData_.end());
	cpuIndices.assign(indices_.begin(), indices_.end());
	vertexStride = vertexStride_;
	vertexCount = vertexCount_;
	indexCount = static_cast<UINT>(cpuIndices.size());

	ID3D12Device* const device{ RenderSystem::GetInstance().GetDevice() };
	if (device == nullptr)
	{
		return true;
	}

	return CreateBuffers(device);
}

bool Mesh::CreateBuffers(ID3D12Device* device_)
{
	if (device_ == nullptr || cpuVertexData.empty() || vertexStride == 0 || vertexCount == 0)
	{
		return false;
	}

	if (HasGpuBuffers())
	{
		return true;
	}

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC vertexBufferDesc{};
	vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexBufferDesc.Width = cpuVertexData.size();
	vertexBufferDesc.Height = 1;
	vertexBufferDesc.DepthOrArraySize = 1;
	vertexBufferDesc.MipLevels = 1;
	vertexBufferDesc.SampleDesc.Count = 1;
	vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	if (FAILED(device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer))))
	{
		return false;
	}

	void* mappedVertexData{ nullptr };
	if (FAILED(vertexBuffer->Map(0, nullptr, &mappedVertexData)))
	{
		Release();
		return false;
	}

	std::memcpy(mappedVertexData, cpuVertexData.data(), cpuVertexData.size());
	vertexBuffer->Unmap(0, nullptr);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast<UINT>(cpuVertexData.size());
	vertexBufferView.StrideInBytes = vertexStride;

	if (!cpuIndices.empty())
	{
		D3D12_RESOURCE_DESC indexBufferDesc{};
		indexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		indexBufferDesc.Width = cpuIndices.size() * sizeof(uint32_t);
		indexBufferDesc.Height = 1;
		indexBufferDesc.DepthOrArraySize = 1;
		indexBufferDesc.MipLevels = 1;
		indexBufferDesc.SampleDesc.Count = 1;
		indexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device_->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&indexBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer))))
		{
			Release();
			return false;
		}

		void* mappedIndexData{ nullptr };
		if (FAILED(indexBuffer->Map(0, nullptr, &mappedIndexData)))
		{
			Release();
			return false;
		}

		std::memcpy(mappedIndexData, cpuIndices.data(), cpuIndices.size() * sizeof(uint32_t));
		indexBuffer->Unmap(0, nullptr);

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(cpuIndices.size() * sizeof(uint32_t));
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	MarkLoaded(true);
	return true;
}

void Mesh::Bind(ID3D12GraphicsCommandList* commandList_) const
{
	if (commandList_ == nullptr || !IsLoaded())
	{
		return;
	}

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
	if (indexCount > 0)
	{
		commandList_->IASetIndexBuffer(&indexBufferView);
	}
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList_) const
{
	if (commandList_ == nullptr || !IsLoaded())
	{
		return;
	}

	if (indexCount > 0)
	{
		commandList_->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		return;
	}

	commandList_->DrawInstanced(vertexCount, 1, 0, 0);
}

UINT Mesh::GetVertexCount() const noexcept
{
	return vertexCount;
}

const Vector3D& Mesh::GetBoundsMin() const noexcept
{
	return boundsMin;
}

const Vector3D& Mesh::GetBoundsMax() const noexcept
{
	return boundsMax;
}

UINT Mesh::GetIndexCount() const noexcept
{
	return indexCount;
}

UINT Mesh::GetVertexStride() const noexcept
{
	return vertexStride;
}

bool Mesh::HasGpuBuffers() const noexcept
{
	return vertexBuffer != nullptr;
}

void Mesh::Release()
{
	vertexBuffer.Reset();
	indexBuffer.Reset();

	vertexBufferView = {};
	indexBufferView = {};
	MarkLoaded(false);
}

bool Mesh::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(tagLength))))
	{
		return false;
	}

	std::string tag(tagLength, '\0');
	if (!static_cast<bool>(file_.read(tag.data(), tagLength)))
	{
		return false;
	}

	return tag == expectedTag_;
}

void Mesh::ReadVector3(std::ifstream& file_, Vector3D& vector_)
{
	file_.read(reinterpret_cast<char*>(&vector_.x), sizeof(float));
	file_.read(reinterpret_cast<char*>(&vector_.y), sizeof(float));
	file_.read(reinterpret_cast<char*>(&vector_.z), sizeof(float));
}
