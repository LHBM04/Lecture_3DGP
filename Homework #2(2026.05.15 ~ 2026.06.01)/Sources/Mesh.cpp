#include "Precompiled.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include <fstream>
#include <string>

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

	auto ReadTag = [&file](const std::string& expectedTag) -> bool {
		uint8_t tagLength = 0;
		if (!file.read(reinterpret_cast<char*>(&tagLength), sizeof(uint8_t))) return false;
		
		std::string tag(tagLength, '\0');
		if (!file.read(&tag[0], tagLength)) return false;

		return tag == expectedTag;
	};

	auto ReadVector3 = [&file](Vector3D& vec) {
		file.read(reinterpret_cast<char*>(&vec.x), sizeof(float));
		file.read(reinterpret_cast<char*>(&vec.y), sizeof(float));
		file.read(reinterpret_cast<char*>(&vec.z), sizeof(float));
	};

	// 1. BoundingBox (Ignore for now)
	if (!ReadTag("<BoundingBox>:")) return false;
	Vector3D min, max;
	ReadVector3(min);
	ReadVector3(max);

	// 2. Positions
	if (!ReadTag("<Positions>:")) return false;
	uint32_t positionCount = 0;
	if (!file.read(reinterpret_cast<char*>(&positionCount), sizeof(uint32_t))) return false;
	
	std::vector<Vector3D> positions(positionCount);
	for (uint32_t i = 0; i < positionCount; ++i)
	{
		ReadVector3(positions[i]);
	}

	// 3. Normals
	if (!ReadTag("<Normals>:")) return false;
	uint32_t normalCount = 0;
	if (!file.read(reinterpret_cast<char*>(&normalCount), sizeof(uint32_t))) return false;
	
	std::vector<Vector3D> normals(normalCount);
	for (uint32_t i = 0; i < normalCount; ++i)
	{
		ReadVector3(normals[i]);
	}

	// 4. TextureCoords (Skip)
	if (!ReadTag("<TextureCoords>:")) return false;
	uint32_t texCoordCount = 0;
	if (!file.read(reinterpret_cast<char*>(&texCoordCount), sizeof(uint32_t))) return false;
	file.seekg(sizeof(float) * 2 * texCoordCount, std::ios::cur);

	// 5. Indices
	if (!ReadTag("<Indices>:")) return false;
	uint32_t indexCount = 0;
	if (!file.read(reinterpret_cast<char*>(&indexCount), sizeof(uint32_t))) return false;
	
	indices.resize(indexCount);
	if (!file.read(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * indexCount)) return false;

	// Combine to Vertices
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

bool Mesh::CreateBuffers(ID3D12Device* device_)
{
	if (vertices.empty())
	{
		return false;
	}

	// Vertex Buffer
	{
		const UINT bufferSize = static_cast<UINT>(vertices.size() * sizeof(Vertex));

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = bufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (FAILED(device_->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer))))
		{
			return false;
		}

		void* pVertexDataBegin;
		D3D12_RANGE readRange{ 0, 0 };
		if (FAILED(vertexBuffer->Map(0, &readRange, &pVertexDataBegin)))
		{
			return false;
		}
		memcpy(pVertexDataBegin, vertices.data(), bufferSize);
		vertexBuffer->Unmap(0, nullptr);

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = bufferSize;
	}

	// Index Buffer
	if (!indices.empty())
	{
		const UINT bufferSize = static_cast<UINT>(indices.size() * sizeof(uint32_t));

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = bufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (FAILED(device_->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer))))
		{
			return false;
		}

		void* pIndexDataBegin;
		D3D12_RANGE readRange{ 0, 0 };
		if (FAILED(indexBuffer->Map(0, &readRange, &pIndexDataBegin)))
		{
			return false;
		}
		memcpy(pIndexDataBegin, indices.data(), bufferSize);
		indexBuffer->Unmap(0, nullptr);

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = bufferSize;
	}

	return true;
}

