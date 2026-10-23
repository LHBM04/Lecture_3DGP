#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	GPUCommandBuffer::~GPUCommandBuffer() noexcept
	{
		if (d3dCommandList)
		{
			d3dCommandList->Release();
			d3dCommandList = nullptr;
		}
	}

	ID3D12GraphicsCommandList10& GPUCommandBuffer::GetD3DCommandList() const noexcept
	{
		return *d3dCommandList;
	}

	GPUCommandBuffer::GPUCommandBuffer(ID3D12GraphicsCommandList10* d3dCommandList) noexcept
		: d3dCommandList(d3dCommandList)
	{
	}
}
