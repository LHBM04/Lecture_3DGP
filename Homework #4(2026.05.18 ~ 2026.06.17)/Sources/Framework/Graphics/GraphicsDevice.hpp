#pragma once

#include <expected>
#include <filesystem>
#include <functional>
#include <memory_resource>
#include <string>
#include <string_view>
#include <vector>

#include <d3d12.h>

#include "CommandBuffer.hpp"
#include "Fence.hpp"
#include "GraphicsPipeline.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

namespace TUK::Framework
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice() noexcept = default;
		explicit GraphicsDevice(
			ID3D12Device10* d3dDevice,
			ID3D12CommandQueue* d3dCommandQueue,
			ID3D12RootSignature* d3dRootSignature) noexcept;
		~GraphicsDevice() noexcept;

		GraphicsDevice(const GraphicsDevice&) noexcept = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) noexcept = delete;

		GraphicsDevice(GraphicsDevice&& other) noexcept;
		GraphicsDevice& operator=(GraphicsDevice&& other) noexcept;

		[[nodiscard]] std::expected<CommandBuffer, std::string> CreateCommandBuffer();
		[[nodiscard]] std::expected<Fence, std::string> CreateFence();
		[[nodiscard]] std::expected<Mesh, std::string> CreateMesh(std::span<const Vertex> vertices, std::span<const uint32_t> indices);
		[[nodiscard]] std::expected<Shader, std::string> CreateShader(const std::filesystem::path& path, std::string_view entry);
		[[nodiscard]] std::expected<GraphicsPipeline, std::string> CreateGraphicsPipeline(const Shader& vs, const Shader& ps);

		[[nodiscard]] std::expected<std::reference_wrapper<CommandBuffer>, std::string> AcquireCommandBuffer();
		[[nodiscard]] std::expected<void, std::string> SubmitCommandBuffer(CommandBuffer& commandBuffer);

		[[nodiscard]] std::expected<std::reference_wrapper<Fence>, std::string> AcquireFence();

		[[nodiscard]] ID3D12Device10& GetD3D12Device() const noexcept;
		[[nodiscard]] ID3D12CommandQueue* GetD3D12CommandQueue() const noexcept;

	private:
		/** D3D12 디바이스 */
		ID3D12Device10* d3dDevice = nullptr;

		/** 명령 큐 */
		ID3D12CommandQueue* d3dCommandQueue = nullptr;

		/** 루트 시그너처 */
		ID3D12RootSignature* d3dRootSignature = nullptr;

		/** 사용할 명령 버퍼들 */
		std::pmr::vector<CommandBuffer> commandBuffers;
		std::size_t currentCommandBufferIndex = 0;

		/** 사용할 펜스들 */
		std::pmr::vector<Fence> fences;
		std::size_t currentFenceIndex = 0;
	};
}
