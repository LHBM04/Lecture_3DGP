#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class GPUDevice;

	class GPUFence
	{
		friend class GPUDevice;

	public:
		~GPUFence() noexcept = default;

		GPUFence(const GPUFence&) = delete;
		GPUFence& operator=(const GPUFence&) = delete;

		GPUFence(GPUFence&&) noexcept = default;
		GPUFence& operator=(GPUFence&&) noexcept = default;

		void Wait() const;
		void Signal(const GPUDevice& device);

		[[nodiscard]] bool IsCompleted() const noexcept;

	private:
		explicit GPUFence(Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence) noexcept;

		Microsoft::WRL::ComPtr<ID3D12Fence> d3dFence;
		HANDLE fenceEvent;
		int fenceValue;
	};
}
