#pragma once

#include <expected>
#include <optional>
#include <string>

#include "Framework/Entities/World.hpp"
#include "Framework/Graphics/GPUBuffer.hpp"
#include "Framework/Graphics/GPUGraphicsPipelineState.hpp"
#include "Framework/Graphics/GPURootSignature.hpp"

namespace TUK::Framework
{
	class GPUDevice;
	class ModelAsset;
	class RenderSubsystem;
}

using namespace TUK::Framework;

namespace TUK::Game
{
	class TestWorld : public World
	{
	public:
		TestWorld() noexcept = default;
		~TestWorld() noexcept override = default;

	protected:
		void OnEnter() override;
		void OnRender(GPUCommandBuffer& renderer) override;
		void OnExit() override;

	private:
		[[nodiscard]] std::expected<void, std::string> CreateTriangle(const GPUDevice& device, DXGI_FORMAT targetFormat);

		RenderSubsystem* renderSubsystem = nullptr;
		std::optional<GPURootSignature> rootSignature;
		std::optional<GPUGraphicsPipelineState> pipelineState;
		ModelAsset* model = nullptr;
		std::optional<GPUBuffer> cameraBuffer;
	};
}
