#pragma once

#include <span>
#include <vector>

#include <d3d12.h>
#include <DirectXMath.h>

#include "CameraConstants.h"
#include "RenderCommand.h"
#include "RenderContext.h"

class Renderer final
{
public:
	Renderer() = default;
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void Clear();
	void SubmitContext(const RenderContext& context_);
	void Execute(ID3D12GraphicsCommandList* cmdList_, RenderPassType targetPassType_);

	[[nodiscard]] std::span<const DrawCall> GetMasterCommands() const noexcept;

private:
	struct RenderBatch final
	{
		uint64_t sortKey{ 0 };
		DrawCall baseCommand{};
		std::vector<DirectX::XMFLOAT4X4> instanceTransforms;
	};

	std::vector<DrawCall> masterCommands;
	std::vector<RenderBatch> batches;
};
