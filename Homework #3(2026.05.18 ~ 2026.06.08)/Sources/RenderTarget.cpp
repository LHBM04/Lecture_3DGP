#include "Precompiled.h"
#include "RenderTarget.h"

void RenderTarget::Initialize(
	Microsoft::WRL::ComPtr<ID3D12Resource> resource,
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
	D3D12_RESOURCE_STATES initialState) noexcept
{
	this->resource = std::move(resource);
	this->rtvHandle = rtvHandle;
	state = initialState;
}

ID3D12Resource* RenderTarget::GetResource() const noexcept
{
	return resource.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRTVHandle() const noexcept
{
	return rtvHandle;
}

D3D12_RESOURCE_STATES RenderTarget::GetState() const noexcept
{
	return state;
}

void RenderTarget::Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES nextState)
{
	if (state == nextState)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = resource.Get();
	barrier.Transition.StateBefore = state;
	barrier.Transition.StateAfter = nextState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	state = nextState;
}

void RenderTarget::Bind(ID3D12GraphicsCommandList* commandList) const
{
	const D3D12_CPU_DESCRIPTOR_HANDLE handle{ rtvHandle };
	commandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);
}

void RenderTarget::Clear(ID3D12GraphicsCommandList* commandList, const FLOAT clearColor[4]) const
{
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void RenderTarget::Reset() noexcept
{
	resource.Reset();
	rtvHandle = {};
	state = D3D12_RESOURCE_STATE_COMMON;
}
