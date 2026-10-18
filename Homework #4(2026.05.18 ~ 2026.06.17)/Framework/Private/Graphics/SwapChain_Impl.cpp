#include "Precompiled.hpp"
#include "SwapChain_Impl.hpp"

#include "Framework/Graphics/CommandBuffer.hpp"
#include "RenderTarget_Impl.hpp"

namespace TUK::Framework
{
	SwapChain_Impl::SwapChain_Impl(IDXGISwapChain4* dxgiSwapChain) noexcept
		: dxgiSwapChain(dxgiSwapChain)
		, renderTargets()
	{
		assert(dxgiSwapChain);

		DXGI_SWAP_CHAIN_DESC1 description{};
		if (SUCCEEDED(dxgiSwapChain->GetDesc1(&description)))
		{
			renderTargets.resize(description.BufferCount);
			CreateRenderTargets();
		}
	}

	SwapChain_Impl::~SwapChain_Impl() noexcept
	{
		ReleaseRenderTargets();

		if (dxgiSwapChain)
		{
			dxgiSwapChain->SetFullscreenState(FALSE, nullptr);
			dxgiSwapChain->Release();
			dxgiSwapChain = nullptr;
		}
	}

	void SwapChain_Impl::Clear(CommandBuffer& commandBuffer) const
	{
		assert(dxgiSwapChain && !renderTargets.empty());

		const UINT backBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
		assert(renderTargets[backBufferIndex]);

		commandBuffer.SetRenderTarget(*renderTargets[backBufferIndex]);
		commandBuffer.ClearRenderTarget(ColorRGBA<float>(0, 0, 0, 0));
	}

	void SwapChain_Impl::Present() const
	{
		assert(dxgiSwapChain);
		dxgiSwapChain->Present(1, 0);
	}

	Vector2D<std::int32_t> SwapChain_Impl::GetSize() const noexcept
	{
		assert(dxgiSwapChain);
		DXGI_SWAP_CHAIN_DESC1 description{};
		dxgiSwapChain->GetDesc1(&description);
		return { static_cast<std::int32_t>(description.Width), static_cast<std::int32_t>(description.Height) };
	}

	void SwapChain_Impl::SetSize(const Vector2D<std::int32_t>& size) noexcept
	{
		assert(dxgiSwapChain);
		if (size.GetX() <= 0 || size.GetY() <= 0)
		{
			return;
		}

		DXGI_SWAP_CHAIN_DESC1 description{};
		if (FAILED(dxgiSwapChain->GetDesc1(&description)))
		{
			return;
		}

		ReleaseRenderTargets();

		if (SUCCEEDED(dxgiSwapChain->ResizeBuffers(
			description.BufferCount,
			static_cast<UINT>(size.GetX()),
			static_cast<UINT>(size.GetY()),
			description.Format,
			description.Flags)))
		{
			CreateRenderTargets();
		}
	}

	IDXGISwapChain4& SwapChain_Impl::GetDXGISwapChain() const noexcept
	{
		assert(dxgiSwapChain);
		return *dxgiSwapChain;
	}

	void SwapChain_Impl::CreateRenderTargets()
	{
		assert(dxgiSwapChain);

		for (UINT index = 0; index < static_cast<UINT>(renderTargets.size()); ++index)
		{
			ID3D12Resource* backBuffer = nullptr;
			if (SUCCEEDED(dxgiSwapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer))) && backBuffer)
			{
				renderTargets[index] = std::make_unique<RenderTarget_Impl>(backBuffer);
			}
		}
	}

	void SwapChain_Impl::ReleaseRenderTargets() noexcept
	{
		for (std::unique_ptr<RenderTarget>& renderTarget : renderTargets)
		{
			renderTarget.reset();
		}
	}
}
