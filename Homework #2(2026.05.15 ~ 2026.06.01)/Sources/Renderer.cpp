#include "Precompiled.h"
#include "Renderer.h"

void Renderer::Clear()
{
	masterCommands.clear();
	batches.clear();
}

void Renderer::SubmitContext(const RenderContext& context_)
{
	const std::span<const RenderCommand> contextCommands{ context_.GetCommands() };
	masterCommands.insert(masterCommands.end(), contextCommands.begin(), contextCommands.end());
}

void Renderer::Execute(ID3D12GraphicsCommandList* cmdList_, RenderPassType targetPassType_)
{
	if (cmdList_ == nullptr)
	{
		return;
	}

	std::vector<RenderCommand> passCommands;
	for (const RenderCommand& command : masterCommands
		| std::views::filter([targetPassType_](const RenderCommand& command_)
			{
				return GetRenderPassType(command_.sortKey) == targetPassType_;
			}))
	{
		passCommands.push_back(command);
	}

	std::ranges::sort(passCommands, {}, &RenderCommand::sortKey);

	batches.clear();
	for (const RenderCommand& command : passCommands)
	{
		if (batches.empty() || batches.back().sortKey != command.sortKey)
		{
			RenderBatch batch{
				.sortKey = command.sortKey,
				.baseCommand = command,
				.instanceTransforms = {},
			};
			batch.instanceTransforms.push_back(command.worldMatrix);
			batches.push_back(std::move(batch));
			continue;
		}

		batches.back().instanceTransforms.push_back(command.worldMatrix);
	}

	for (const RenderBatch& batch : batches)
	{
		const RenderCommand& command{ batch.baseCommand };
		if (command.vertexBufferViewPtr == nullptr || command.indexBufferViewPtr == nullptr)
		{
			continue;
		}

		const auto* vertexBufferView{ static_cast<const D3D12_VERTEX_BUFFER_VIEW*>(command.vertexBufferViewPtr) };
		const auto* indexBufferView{ static_cast<const D3D12_INDEX_BUFFER_VIEW*>(command.indexBufferViewPtr) };

		cmdList_->IASetVertexBuffers(0, 1, vertexBufferView);
		cmdList_->IASetIndexBuffer(indexBufferView);
		cmdList_->DrawIndexedInstanced(
			command.indexCount,
			static_cast<UINT>(batch.instanceTransforms.size()),
			command.startIndexLocation,
			command.baseVertexLocation,
			0);
	}
}

std::span<const RenderCommand> Renderer::GetMasterCommands() const noexcept
{
	return masterCommands;
}
