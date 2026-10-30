#pragma once

#include <expected>
#include <optional>
#include <string>

#include "Framework/Entities/World.hpp"
#include "Framework/Graphics/D3DBuffer.hpp"
#include "Framework/Graphics/D3DGraphicsPipelineState.hpp"
#include "Framework/Graphics/D3DRootSignature.hpp"

namespace TUK::Framework
{
	class D3DDevice;
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
		void OnRender(D3DCommandBuffer& renderer) override;
		void OnExit() override;

	private:
		[[nodiscard]] std::expected<void, std::string> CreateTriangle(const D3DDevice& device, DXGI_FORMAT targetFormat);

		RenderSubsystem* renderSubsystem = nullptr;
		std::optional<D3DRootSignature> rootSignature;
		std::optional<D3DGraphicsPipelineState> pipelineState;
		ModelAsset* model = nullptr;
		std::optional<D3DBuffer> cameraBuffer;
	};
}
