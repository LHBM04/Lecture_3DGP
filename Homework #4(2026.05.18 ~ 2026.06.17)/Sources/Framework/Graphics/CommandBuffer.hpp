#pragma once

#include <functional>
#include <span>
#include <vector>

#include <d3d12.h>

#include "../Math/ColorRGB.hpp"
#include "../Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	class RenderSubsystem;
	class RenderTarget;
	class Texture;
	class Shader;
	class Buffer;

	class CommandBuffer
	{
		friend class RenderSubsystem;

	public:
		CommandBuffer(ID3D12CommandAllocator* d3dCommandAllocator, ID3D12GraphicsCommandList* d3dCommandList) noexcept;
		~CommandBuffer() noexcept;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		CommandBuffer(CommandBuffer&&) = delete;
		CommandBuffer& operator=(CommandBuffer&&) = delete;

		void BeginFrame();
		void EndFrame();

		void SetRenderTarget(std::reference_wrapper<RenderTarget> renderTarget);
		void SetRenderTargets(std::span<std::reference_wrapper<RenderTarget>> renderTargets);

		void ClearRenderTarget(const ColorRGB<float>& clearColor) const;
		void ClearRenderTarget(const ColorRGBA<float>& clearColor) const;

		/** D3D12 커맨드 할당자 Getter */
		[[nodiscard]] ID3D12CommandAllocator& GetD3D12CommandAllocator() const noexcept;

		/** D3D12 커맨드 리스트 Getter */
		[[nodiscard]] ID3D12GraphicsCommandList& GetD3D12CommandList() const noexcept;

	private:
		ID3D12CommandAllocator* d3dCommandAllocator;
		ID3D12GraphicsCommandList* d3dCommandList;
		std::vector<std::reference_wrapper<RenderTarget>> currentRenderTargets;
	};
}
