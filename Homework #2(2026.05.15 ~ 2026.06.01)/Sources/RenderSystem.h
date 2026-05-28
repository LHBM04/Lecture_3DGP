#pragma once

#include "System.h"
#include "Renderer.h"
#include "RendererOptions.h";

class RenderSystem : public ISystem
{
public:
	bool Initialize();
	void Release() override;

	void BeginRender();
	void Render();
	void EndRender();

	std::expected<Renderer*, std::wstring> CreateRenderer(const RendererOptions& options_);
	void DestroyRenderer(Renderer* renderer_);

private:
#if defined (_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController{ nullptr };
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Device> device{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent{ nullptr };
	UINT64 fenceValue{ 1 };

	std::vector<std::unique_ptr<Renderer>> renderers;
};
