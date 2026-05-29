#include "Precompiled.h"
#include "Mesh.h"

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
