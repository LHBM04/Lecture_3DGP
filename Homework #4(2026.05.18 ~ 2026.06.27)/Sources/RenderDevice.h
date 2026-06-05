#pragma once

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

class RenderDevice
{
public:
	RenderDevice() noexcept = default;
	~RenderDevice() noexcept = default;

	bool Initialize();
	void Release();

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] IDXGIFactory4* GetFactory() const noexcept;

private:
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	bool isEnableMsaa4x{ false };
	UINT msaa4xQualityLevels{ 0 };
};
