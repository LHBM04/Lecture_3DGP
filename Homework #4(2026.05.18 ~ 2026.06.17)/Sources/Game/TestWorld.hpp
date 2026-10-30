#pragma once

#include <expected>
#include <optional>
#include <string>

#include "Framework/Entities/World.hpp"
#include "Framework/Graphics/GPUBuffer.hpp"
#include "Framework/Graphics/GPUGraphicsPipelineState.hpp"

namespace TUK::Framework
{
	class GPUDevice;
	class MaterialAsset;
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

		[[nodiscard]] std::expected<GPUGraphicsPipelineState, std::string> CreateMaterialPipeline(
			const GPUDevice& device, const MaterialAsset& material, DXGI_FORMAT targetFormat) const;

		RenderSubsystem* renderSubsystem = nullptr;
		std::optional<GPUGraphicsPipelineState> pipelineState;
		MaterialAsset* material = nullptr;
		ModelAsset* model = nullptr;
		std::optional<GPUBuffer> cameraBuffer;
	};
}
