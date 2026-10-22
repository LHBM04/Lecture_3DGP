#include "Precompiled.hpp"
#include "SwapChain.hpp"

#include "RenderTarget.hpp"

namespace TUK::Framework
{
	SwapChain::SwapChain(IDXGISwapChain4* dxgiSwapChain) noexcept
		: dxgiSwapChain(dxgiSwapChain)
		, renderTargets()
	{
		assert(dxgiSwapChain);
		CreateRenderTargets();
	}

	SwapChain::~SwapChain() noexcept
	{
		ReleaseRenderTargets();

		if (dxgiSwapChain)
		{
			dxgiSwapChain->SetFullscreenState(FALSE, nullptr);
			dxgiSwapChain->Release();
			dxgiSwapChain = nullptr;
		}
	}

	SwapChain::SwapChain(SwapChain&& other) noexcept
		: dxgiSwapChain(std::exchange(other.dxgiSwapChain, nullptr))
		, renderTargets(std::move(other.renderTargets))
	{
	}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		if (this != &other)
		{
			ReleaseRenderTargets();

			if (dxgiSwapChain)
			{
				dxgiSwapChain->SetFullscreenState(FALSE, nullptr);
				dxgiSwapChain->Release();
			}

			dxgiSwapChain = std::exchange(other.dxgiSwapChain, nullptr);
			renderTargets = std::move(other.renderTargets);
		}
		return *this;
	}

	void SwapChain::Present() const
	{
		assert(dxgiSwapChain);
		dxgiSwapChain->Present(1, 0);
	}

	RenderTarget& SwapChain::GetCurrentRenderTarget()
	{
		assert(dxgiSwapChain && !renderTargets.empty());

		const UINT backBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
		assert(backBufferIndex < renderTargets.size());

		return renderTargets[backBufferIndex];
	}

	const RenderTarget& SwapChain::GetCurrentRenderTarget() const
	{
		assert(dxgiSwapChain && !renderTargets.empty());

		const UINT backBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
		assert(backBufferIndex < renderTargets.size());

		return renderTargets[backBufferIndex];
	}

	Vector2D<std::int32_t> SwapChain::GetSize() const noexcept
	{
		assert(dxgiSwapChain);
		DXGI_SWAP_CHAIN_DESC1 description{};
		dxgiSwapChain->GetDesc1(&description);
		return { static_cast<std::int32_t>(description.Width), static_cast<std::int32_t>(description.Height) };
	}

	void SwapChain::SetSize(const Vector2D<std::int32_t>& size) noexcept
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

	IDXGISwapChain4& SwapChain::GetDXGISwapChain() const noexcept
	{
		assert(dxgiSwapChain);
		return *dxgiSwapChain;
	}

	void SwapChain::CreateRenderTargets()
	{
		assert(dxgiSwapChain);

		DXGI_SWAP_CHAIN_DESC1 description{};
		if (FAILED(dxgiSwapChain->GetDesc1(&description)))
		{
			return;
		}

		renderTargets.clear();
		renderTargets.reserve(description.BufferCount);

		for (UINT index = 0; index < description.BufferCount; ++index)
		{
			ID3D12Resource* backBuffer = nullptr;
			if (SUCCEEDED(dxgiSwapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer))) && backBuffer)
			{
				renderTargets.emplace_back(backBuffer);
			}
		}
	}

	void SwapChain::ReleaseRenderTargets() noexcept
	{
		renderTargets.clear();
	}
}
