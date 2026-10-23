#pragma once

namespace TUK::Framework
{
	class GPUCommandBuffer
	{
	public:
		~GPUCommandBuffer() noexcept;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = delete;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = delete;

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;

	private:
		explicit GPUCommandBuffer(ID3D12GraphicsCommandList10* d3dCommandList) noexcept;

		ID3D12GraphicsCommandList10* d3dCommandList;
	};
}
