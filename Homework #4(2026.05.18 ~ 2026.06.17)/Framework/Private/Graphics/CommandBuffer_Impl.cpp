#include "Precompiled.hpp"
#include "CommandBuffer_Impl.hpp"

#include "RenderTarget_Impl.hpp"

namespace TUK::Framework
{
	CommandBuffer_Impl::CommandBuffer_Impl(ID3D12CommandAllocator* d3dCommandAllocator, ID3D12GraphicsCommandList* d3dCommandList) noexcept
		: d3dCommandAllocator(d3dCommandAllocator)
		, d3dCommandList(d3dCommandList)
	{
		assert(d3dCommandAllocator && d3dCommandList);
	}

	CommandBuffer_Impl::~CommandBuffer_Impl() noexcept
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

	void CommandBuffer_Impl::BeginFrame()
	{
		assert(d3dCommandAllocator && d3dCommandList);

		currentRenderTargets.clear();

		if (FAILED(d3dCommandAllocator->Reset()))
		{
			return;
		}

		d3dCommandList->Reset(d3dCommandAllocator, nullptr);
	}

	void CommandBuffer_Impl::EndFrame()
	{
		assert(d3dCommandAllocator && d3dCommandList);

		if (!currentRenderTargets.empty())
		{
			std::vector<D3D12_RESOURCE_BARRIER> barriers;
			barriers.reserve(currentRenderTargets.size());

			for (const std::reference_wrapper<RenderTarget>& renderTarget : currentRenderTargets)
			{
				RenderTarget_Impl& renderTargetImpl = dynamic_cast<RenderTarget_Impl&>(renderTarget.get());

				D3D12_RESOURCE_BARRIER barrier{};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Transition.pResource = &renderTargetImpl.GetD3D12Resource();
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
				barriers.push_back(barrier);
			}

			d3dCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
			currentRenderTargets.clear();
		}

		d3dCommandList->Close();
	}

	void CommandBuffer_Impl::SetRenderTarget(std::reference_wrapper<RenderTarget> renderTarget)
	{
		SetRenderTargets(std::span<std::reference_wrapper<RenderTarget>>(&renderTarget, 1));
	}

	void CommandBuffer_Impl::SetRenderTargets(std::span<std::reference_wrapper<RenderTarget>> renderTargets)
	{
		assert(d3dCommandList);

		if (!currentRenderTargets.empty())
		{
			std::vector<D3D12_RESOURCE_BARRIER> barriers;
			barriers.reserve(currentRenderTargets.size());

			for (const std::reference_wrapper<RenderTarget>& renderTarget : currentRenderTargets)
			{
				RenderTarget_Impl& renderTargetImpl = dynamic_cast<RenderTarget_Impl&>(renderTarget.get());

				D3D12_RESOURCE_BARRIER barrier{};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Transition.pResource = &renderTargetImpl.GetD3D12Resource();
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
				barriers.push_back(barrier);
			}

			d3dCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
			currentRenderTargets.clear();
		}

		if (renderTargets.empty())
		{
			d3dCommandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
			return;
		}

		std::vector<D3D12_RESOURCE_BARRIER> barriers;
		barriers.reserve(renderTargets.size());

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
		rtvHandles.reserve(renderTargets.size());
		
		currentRenderTargets.reserve(renderTargets.size());

		for (const std::reference_wrapper<RenderTarget>& renderTarget : renderTargets)
		{
			RenderTarget_Impl& renderTargetImpl = dynamic_cast<RenderTarget_Impl&>(renderTarget.get());

			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = &renderTargetImpl.GetD3D12Resource();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barriers.push_back(barrier);

			rtvHandles.push_back(renderTargetImpl.GetD3D12RTVHandle());
			currentRenderTargets.push_back(renderTarget);
		}

		d3dCommandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
		d3dCommandList->OMSetRenderTargets(static_cast<UINT>(rtvHandles.size()), rtvHandles.data(), FALSE, nullptr);
	}

	void CommandBuffer_Impl::ClearRenderTarget(const ColorRGB<float>& clearColor) const
	{
		ClearRenderTarget(clearColor.ToColorRGBA());
	}

	void CommandBuffer_Impl::ClearRenderTarget(const ColorRGBA<float>& clearColor) const
	{
		assert(d3dCommandList);

		const FLOAT color[4]{ clearColor.GetR(), clearColor.GetG(), clearColor.GetB(), clearColor.GetA() };
		for (const std::reference_wrapper<RenderTarget>& renderTarget : currentRenderTargets)
		{
			const RenderTarget_Impl& renderTargetImpl = dynamic_cast<const RenderTarget_Impl&>(renderTarget.get());
			d3dCommandList->ClearRenderTargetView(renderTargetImpl.GetD3D12RTVHandle(), color, 0, nullptr);
		}
	}

	ID3D12CommandAllocator& CommandBuffer_Impl::GetD3D12CommandAllocator() const noexcept
	{
		assert(d3dCommandAllocator);
		return *d3dCommandAllocator;
	}

	ID3D12GraphicsCommandList& CommandBuffer_Impl::GetD3D12CommandList() const noexcept
	{
		assert(d3dCommandList);
		return *d3dCommandList;
	}
}
