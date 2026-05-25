#include "Precompiled.h"
#include "Mesh.h"

#include "Application.h"

namespace
{
	bool ReadSectionName(const std::vector<std::byte>& bytes_, std::size_t& offset_, std::string& name_)
	{
		if (offset_ >= bytes_.size())
		{
			return false;
		}

		const std::size_t length{ std::to_integer<std::size_t>(bytes_[offset_]) };
		++offset_;

		if (offset_ + length > bytes_.size())
		{
			return false;
		}

		name_.assign(
			reinterpret_cast<const char*>(bytes_.data() + offset_),
			length);
		offset_ += length;
		return true;
	}

	template <typename T>
	bool ReadValue(const std::vector<std::byte>& bytes_, std::size_t& offset_, T& value_)
	{
		if (offset_ + sizeof(T) > bytes_.size())
		{
			return false;
		}

		std::memcpy(&value_, bytes_.data() + offset_, sizeof(T));
		offset_ += sizeof(T);
		return true;
	}

	bool CreateUploadBuffer(ID3D12Device* device_, const void* data_, std::size_t byteSize_, Microsoft::WRL::ComPtr<ID3D12Resource>& resource_)
	{
		if (nullptr == device_ || nullptr == data_ || 0 == byteSize_)
		{
			return false;
		}

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDescription{};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Width = static_cast<UINT64>(byteSize_);
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(device_->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&resource_))))
		{
			return false;
		}

		void* mapped{ nullptr };
		if (FAILED(resource_->Map(0, nullptr, &mapped)))
		{
			resource_.Reset();
			return false;
		}

		std::memcpy(mapped, data_, byteSize_);
		resource_->Unmap(0, nullptr);
		return true;
	}
}

bool Mesh::BuildFromRaw(
	ID3D12Device* device_,
	std::span<const Vertex> vertices_,
	std::span<const std::uint32_t> indices_)
{
	if (nullptr == device_ || vertices_.empty())
	{
		return false;
	}

	Unload();

	const std::size_t vertexByteSize{ vertices_.size() * sizeof(Vertex) };
	if (!CreateUploadBuffer(device_, vertices_.data(), vertexByteSize, vertexBuffer))
	{
		return false;
	}

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast<UINT>(vertexByteSize);
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexCount = static_cast<UINT>(vertices_.size());

	if (!indices_.empty())
	{
		const std::size_t indexByteSize{ indices_.size() * sizeof(std::uint32_t) };
		if (!CreateUploadBuffer(device_, indices_.data(), indexByteSize, indexBuffer))
		{
			return false;
		}

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(indexByteSize);
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		hasIndexBuffer = true;
		indexCount = static_cast<UINT>(indices_.size());
	}

	SetLoaded(true);
	return true;
}

bool Mesh::Load(const std::filesystem::path& path_)
{
	std::ifstream file{ path_, std::ios::binary };
	if (!file)
	{
		return false;
	}

	std::vector<char> rawBytes{
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};
	std::vector<std::byte> bytes(rawBytes.size());
	std::memcpy(bytes.data(), rawBytes.data(), rawBytes.size());

	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<std::uint32_t> indices;

	std::size_t offset{ 0 };
	while (offset < bytes.size())
	{
		std::string name;
		if (!ReadSectionName(bytes, offset, name))
		{
			return false;
		}

		if ("<BoundingBox>:" == name)
		{
			constexpr std::size_t boundingBoxFloatCount{ 6 };
			offset += sizeof(float) * boundingBoxFloatCount;
			continue;
		}

		std::uint32_t count{ 0 };
		if (!ReadValue(bytes, offset, count))
		{
			return false;
		}

		if ("<Positions>:" == name)
		{
			positions.resize(static_cast<std::size_t>(count) * 3u);
			const std::size_t byteSize{ positions.size() * sizeof(float) };
			if (offset + byteSize > bytes.size())
			{
				return false;
			}

			std::memcpy(positions.data(), bytes.data() + offset, byteSize);
			offset += byteSize;
			continue;
		}

		if ("<Normals>:" == name)
		{
			normals.resize(static_cast<std::size_t>(count) * 3u);
			const std::size_t byteSize{ normals.size() * sizeof(float) };
			if (offset + byteSize > bytes.size())
			{
				return false;
			}

			std::memcpy(normals.data(), bytes.data() + offset, byteSize);
			offset += byteSize;
			continue;
		}

		if ("<TextureCoords>:" == name)
		{
			texCoords.resize(static_cast<std::size_t>(count) * 2u);
			const std::size_t byteSize{ texCoords.size() * sizeof(float) };
			if (offset + byteSize > bytes.size())
			{
				return false;
			}

			std::memcpy(texCoords.data(), bytes.data() + offset, byteSize);
			offset += byteSize;
			continue;
		}

		if ("<Indices>:" == name)
		{
			indices.resize(count);
			const std::size_t byteSize{ indices.size() * sizeof(std::uint32_t) };
			if (offset + byteSize > bytes.size())
			{
				return false;
			}

			std::memcpy(indices.data(), bytes.data() + offset, byteSize);
			offset += byteSize;
			continue;
		}

		return false;
	}

	const std::size_t vertexCountValue{ positions.size() / 3u };
	if (0 == vertexCountValue)
	{
		return false;
	}

	std::vector<Vertex> vertices(vertexCountValue);
	for (std::size_t index{ 0 }; index < vertexCountValue; ++index)
	{
		vertices[index].position[0] = positions[index * 3u + 0u];
		vertices[index].position[1] = positions[index * 3u + 1u];
		vertices[index].position[2] = positions[index * 3u + 2u];

		if (normals.size() >= (index + 1u) * 3u)
		{
			vertices[index].normal[0] = normals[index * 3u + 0u];
			vertices[index].normal[1] = normals[index * 3u + 1u];
			vertices[index].normal[2] = normals[index * 3u + 2u];
		}

		if (texCoords.size() >= (index + 1u) * 2u)
		{
			vertices[index].texCoord[0] = texCoords[index * 2u + 0u];
			vertices[index].texCoord[1] = texCoords[index * 2u + 1u];
		}
	}

	auto device{ Application::GetRenderer().GetDevice() };

	const std::size_t vertexByteSize{ vertices.size() * sizeof(Vertex) };
	if (!CreateUploadBuffer(device, vertices.data(), vertexByteSize, vertexBuffer))
	{
		return false;
	}

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast<UINT>(vertexByteSize);
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexCount = static_cast<UINT>(vertices.size());

	if (!indices.empty())
	{
		const std::size_t indexByteSize{ indices.size() * sizeof(std::uint32_t) };
		if (!CreateUploadBuffer(device, indices.data(), indexByteSize, indexBuffer))
		{
			return false;
		}

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(indexByteSize);
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		hasIndexBuffer = true;
		indexCount = static_cast<UINT>(indices.size());
	}

	SetPath(path_);
	SetLoaded(true);
	return true;
}

bool Mesh::LoadFromBinary(ID3D12Device* device_, const std::filesystem::path& path_)
{
	if (nullptr == device_)
	{
		return false;
	}

	return Load(path_);
}

void Mesh::Unload()
{
	vertexBuffer.Reset();
	indexBuffer.Reset();
	vertexBufferView = {};
	indexBufferView = {};
	hasIndexBuffer = false;
	vertexCount = 0;
	indexCount = 0;
	SetLoaded(false);
}

uint64_t Mesh::GetId() const noexcept
{
	return id;
}

void Mesh::SetId(uint64_t id_) noexcept
{
	id = id_;
}

const D3D12_VERTEX_BUFFER_VIEW& Mesh::GetVertexBufferView() const noexcept
{
	return vertexBufferView;
}

void Mesh::SetVertexBufferView(const D3D12_VERTEX_BUFFER_VIEW& view_) noexcept
{
	vertexBufferView = view_;
}

bool Mesh::HasIndexBuffer() const noexcept
{
	return hasIndexBuffer;
}

const D3D12_INDEX_BUFFER_VIEW& Mesh::GetIndexBufferView() const noexcept
{
	return indexBufferView;
}

void Mesh::SetIndexBufferView(const D3D12_INDEX_BUFFER_VIEW& view_) noexcept
{
	indexBufferView = view_;
	hasIndexBuffer = true;
}

void Mesh::ClearIndexBufferView() noexcept
{
	indexBufferView = {};
	hasIndexBuffer = false;
}

UINT Mesh::GetVertexCount() const noexcept
{
	return vertexCount;
}

void Mesh::SetVertexCount(UINT count_) noexcept
{
	vertexCount = count_;
}

UINT Mesh::GetIndexCount() const noexcept
{
	return indexCount;
}

void Mesh::SetIndexCount(UINT count_) noexcept
{
	indexCount = count_;
}


