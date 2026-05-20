#pragma once

class Renderer;
struct RendererOptions;

class RenderSystem
{
	friend class Renderer;
	STATIC_CLASS(RenderSystem);

	[[nodiscard]] static bool Initialize();
	static void Shutdown() noexcept;

	static std::unique_ptr<Renderer> CreateRenderer(const RendererOptions& options_);

private:
	[[nodiscard]] static bool CheckTearingSupport();
	[[nodiscard]] static bool CreateMeshPipeline();

	static Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	static Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> meshRootSignature;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> meshPipelineState;
	static bool tearingSupported;
};
