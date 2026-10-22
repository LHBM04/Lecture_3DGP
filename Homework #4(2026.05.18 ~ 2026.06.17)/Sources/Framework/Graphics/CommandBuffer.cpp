#include "Precompiled.hpp"
#include "CommandBuffer.hpp"

#include <stdexcept>

#include "RenderPassInfo.hpp"
#include "RenderTarget.hpp"

namespace
{
	D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE ToD3D12(TUK::Framework::RenderPassBeginningOperation operation)
	{
		using TUK::Framework::RenderPassBeginningOperation;
		switch (operation)
		{
		case RenderPassBeginningOperation::Preserve:
			return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
		case RenderPassBeginningOperation::Clear:
			return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		case RenderPassBeginningOperation::Discard:
			return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		}
		throw std::invalid_argument("Invalid render pass beginning operation.");
	}

	D3D12_RENDER_PASS_ENDING_ACCESS_TYPE ToD3D12(TUK::Framework::RenderPassEndingOperation operation)
	{
		using TUK::Framework::RenderPassEndingOperation;
		switch (operation)
		{
		case RenderPassEndingOperation::Preserve:
			return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
		case RenderPassEndingOperation::Discard:
			return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		}
		throw std::invalid_argument("Invalid render pass ending operation.");
	}

	D3D12_RESOURCE_STATES ToD3D12(TUK::Framework::RenderPassState state)
	{
		using TUK::Framework::RenderPassState;
		switch (state)
		{
		case RenderPassState::Present:
			return D3D12_RESOURCE_STATE_PRESENT;
		case RenderPassState::RenderTarget:
			return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case RenderPassState::ShaderResource:
			return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		case RenderPassState::CopySource:
			return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case RenderPassState::CopyDestination:
			return D3D12_RESOURCE_STATE_COPY_DEST;
		case RenderPassState::Common:
			return D3D12_RESOURCE_STATE_COMMON;
		}
		throw std::invalid_argument("Invalid render pass resource state.");
	}
}

namespace TUK::Framework
{
	CommandBuffer::CommandBuffer(ID3D12CommandAllocator* d3dCommandAllocator, ID3D12GraphicsCommandList4* d3dCommandList) noexcept
		: d3dCommandAllocator(d3dCommandAllocator)
		, d3dCommandList(d3dCommandList)
	{
		assert(d3dCommandAllocator && d3dCommandList);
	}

	CommandBuffer::~CommandBuffer() noexcept
	{
		currentRenderTargets.clear();

		if (d3dCommandList)
		{
			d3dCommandList->Release();
			d3dCommandList = nullptr;
		}

		if (d3dCommandAllocator)
		{
			d3dCommandAllocator->Release();
			d3dCommandAllocator = nullptr;
		}
	}

	CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
		: d3dCommandAllocator(std::exchange(other.d3dCommandAllocator, nullptr))
		, d3dCommandList(std::exchange(other.d3dCommandList, nullptr))
		, currentRenderTargets(std::move(other.currentRenderTargets))
		, stateAfterRenderPass(other.stateAfterRenderPass)
	{
	}

	CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
	{
		if (this != &other)
		{
			currentRenderTargets.clear();

			if (d3dCommandList)
			{
				d3dCommandList->Release();
			}

			if (d3dCommandAllocator)
			{
				d3dCommandAllocator->Release();
			}

			d3dCommandAllocator = std::exchange(other.d3dCommandAllocator, nullptr);
			d3dCommandList = std::exchange(other.d3dCommandList, nullptr);
			currentRenderTargets = std::move(other.currentRenderTargets);
			stateAfterRenderPass = other.stateAfterRenderPass;
		}
		return *this;
	}

	void CommandBuffer::BeginRenderPass(const RenderPassInfo& renderPassInfo)
	{
		assert(d3dCommandList);

		const D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE operationBefore = ToD3D12(renderPassInfo.operationBefore);
		const D3D12_RENDER_PASS_ENDING_ACCESS_TYPE operationAfter = ToD3D12(renderPassInfo.operationAfter);
		const D3D12_RESOURCE_STATES stateBefore = ToD3D12(renderPassInfo.stateBefore);
		const D3D12_RESOURCE_STATES stateAfter = ToD3D12(renderPassInfo.stateAfter);

		std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> descriptions{};
		std::array<D3D12_RESOURCE_BARRIER, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> barriers{};
		UINT barrierCount = 0;

		for (std::size_t index = 0; index < renderPassInfo.renderTargets.size(); ++index)
		{
			RenderTarget& renderTarget = renderPassInfo.renderTargets[index].get();
			if (stateBefore != D3D12_RESOURCE_STATE_RENDER_TARGET)
			{
				D3D12_RESOURCE_BARRIER& barrier = barriers[barrierCount++];
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Transition.pResource = &renderTarget.GetD3D12Resource();
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier.Transition.StateBefore = stateBefore;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			}

			D3D12_RENDER_PASS_RENDER_TARGET_DESC& description = descriptions[index];
			description.cpuDescriptor = renderTarget.GetD3D12RTVHandle();
			description.BeginningAccess.Type = operationBefore;
			if (operationBefore == D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR)
			{
				description.BeginningAccess.Clear.ClearValue.Format = renderTarget.GetD3D12Resource().GetDesc().Format;
				description.BeginningAccess.Clear.ClearValue.Color[0] = renderPassInfo.clearColor.GetR();
				description.BeginningAccess.Clear.ClearValue.Color[1] = renderPassInfo.clearColor.GetG();
				description.BeginningAccess.Clear.ClearValue.Color[2] = renderPassInfo.clearColor.GetB();
				description.BeginningAccess.Clear.ClearValue.Color[3] = renderPassInfo.clearColor.GetA();
			}
			description.EndingAccess.Type = operationAfter;
		}

		currentRenderTargets.assign(renderPassInfo.renderTargets.begin(), renderPassInfo.renderTargets.end());
		stateAfterRenderPass = stateAfter;

		if (barrierCount > 0)
		{
			d3dCommandList->ResourceBarrier(barrierCount, barriers.data());
		}

		d3dCommandList->BeginRenderPass(
			static_cast<UINT>(renderPassInfo.renderTargets.size()),
			descriptions.data(),
			nullptr, 
			D3D12_RENDER_PASS_FLAG_NONE);
	}

	void CommandBuffer::EndRenderPass()
	{
		assert(d3dCommandList);
		if (currentRenderTargets.empty())
		{
			throw std::logic_error("No render pass is active.");
		}

		d3dCommandList->EndRenderPass();

		if (stateAfterRenderPass != D3D12_RESOURCE_STATE_RENDER_TARGET)
		{
			std::array<D3D12_RESOURCE_BARRIER, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> barriers{};
			for (std::size_t index = 0; index < currentRenderTargets.size(); ++index)
			{
				D3D12_RESOURCE_BARRIER& barrier = barriers[index];
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Transition.pResource = &currentRenderTargets[index].get().GetD3D12Resource();
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = stateAfterRenderPass;
			}

			d3dCommandList->ResourceBarrier(static_cast<UINT>(currentRenderTargets.size()), barriers.data());
		}
		currentRenderTargets.clear();
	}

	void CommandBuffer::DrawInstanced(
		UINT vertexCountPerInstance, 
		UINT instanceCount, 
		UINT startVertexLocation,
		UINT startInstanceLocation)
	{
		assert(d3dCommandList);
		d3dCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}

	ID3D12CommandAllocator& CommandBuffer::GetD3D12CommandAllocator() const noexcept
	{
		assert(d3dCommandAllocator);
		return *d3dCommandAllocator;
	}

	ID3D12GraphicsCommandList4& CommandBuffer::GetD3D12CommandList() const noexcept
	{
		assert(d3dCommandList);
		return *d3dCommandList;
	}
}
