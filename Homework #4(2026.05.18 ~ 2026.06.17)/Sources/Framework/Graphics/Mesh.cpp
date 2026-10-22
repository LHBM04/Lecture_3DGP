#include "Precompiled.hpp"
#include "Mesh.hpp"

namespace TUK::Framework
{
	Mesh::Mesh(
		ID3D12Resource2* vertexBuffer, 
		ID3D12Resource2* indexBuffer) noexcept
		: vertexBuffer(vertexBuffer)
		, indexBuffer(indexBuffer)
	{
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBuffer->GetDesc().Width);
		vertexBufferView.StrideInBytes = sizeof(float) * 3;

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = static_cast<UINT>(indexBuffer->GetDesc().Width);
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	Mesh::~Mesh() noexcept
	{
		if (vertexBuffer)
		{
			vertexBuffer->Release();
			vertexBuffer = nullptr;
		}

		if (indexBuffer)
		{
			indexBuffer->Release();
			indexBuffer = nullptr;
		}
	}

	Mesh::Mesh(Mesh&& other) noexcept
		: vertexBuffer(std::exchange(other.vertexBuffer, nullptr))
		, vertexBufferView(other.vertexBufferView)
		, indexBuffer(std::exchange(other.indexBuffer, nullptr))
		, indexBufferView(other.indexBufferView)
	{
	}

	Mesh& Mesh::operator=(Mesh&& other) noexcept
	{
		if (this != &other)
		{
			if (vertexBuffer)
			{
				vertexBuffer->Release();
			}
			if (indexBuffer)
			{
				indexBuffer->Release();
			}

			vertexBuffer = std::exchange(other.vertexBuffer, nullptr);
			vertexBufferView = other.vertexBufferView;
			
			indexBuffer = std::exchange(other.indexBuffer, nullptr);
			indexBufferView = other.indexBufferView;
		}
		return *this;
	}
}
