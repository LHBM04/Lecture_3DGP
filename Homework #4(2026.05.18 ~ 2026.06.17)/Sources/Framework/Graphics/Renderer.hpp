#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <d3d12.h>
#include <wrl.h>
#include "../Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	class RenderSubsystem;
	class RenderTarget;
	class Texture;
	class Shader;
	class Buffer;

	class Renderer final
	{
		friend class RenderSubsystem;

	public:
		Renderer() noexcept = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

		void SetRenderTarget(RenderTarget& target);
		void Transition(Texture& texture, D3D12_RESOURCE_STATES nextState);
		void Clear(const ColorRGBA<float>& color, float depth = 1.0f);
		void SetShader(const Shader& shader);
		void SetTexture(UINT parameterIndex, Texture& texture);
		void SetVertexBuffer(const Buffer& buffer, UINT stride, UINT slot = 0);
		void SetIndexBuffer(const Buffer& buffer, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
		void SetConstants(UINT parameterIndex, std::span<const std::uint32_t> values, UINT offset = 0);
		void SetConstantBuffer(UINT parameterIndex, const Buffer& buffer);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
		void Draw(UINT vertexCount, UINT startVertex = 0, UINT instanceCount = 1);
		void DrawIndexed(UINT indexCount, UINT startIndex = 0, INT baseVertex = 0, UINT instanceCount = 1);
		void Dispatch(UINT x, UINT y, UINT z);
		void Blit(Texture& source, const Shader& shader);
		[[nodiscard]] ID3D12GraphicsCommandList& GetCommandList() const;
		[[nodiscard]] bool IsRecording() const noexcept { return isRecording; }

	private:
		std::expected<void, std::string> Initialize(ID3D12Device& device);
		std::expected<void, std::string> Begin();
		std::expected<void, std::string> End();
		void Release() noexcept;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator{};
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList{};
		RenderTarget* currentTarget = nullptr;
		bool isRecording = false;
		bool hasShader = false;
		bool isComputeShader = false;
	};
}
