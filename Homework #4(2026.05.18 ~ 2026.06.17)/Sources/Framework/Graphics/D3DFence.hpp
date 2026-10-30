#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class D3DDevice;

	class D3DFence
	{
		friend class D3DDevice;

	public:
		~D3DFence() noexcept = default;

		D3DFence(const D3DFence&) = delete;
		D3DFence& operator=(const D3DFence&) = delete;

		D3DFence(D3DFence&&) noexcept = default;
		D3DFence& operator=(D3DFence&&) noexcept = default;

		[[nodiscard]] std::expected<void, std::string> Wait() const;
		[[nodiscard]] std::expected<void, std::string> Signal(const D3DDevice& device);

		[[nodiscard]] bool IsCompleted() const noexcept;

	private:
		explicit D3DFence(Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		std::uint64_t fenceValue;
	};
}
