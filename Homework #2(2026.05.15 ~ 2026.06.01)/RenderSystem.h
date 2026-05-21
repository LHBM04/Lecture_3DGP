#pragma once

class Renderer;
class Material;
struct RendererOptions;

class RenderSystem final
{
	friend class Renderer;
	STATIC_CLASS(RenderSystem);

	[[nodiscard]] static bool Initialize();
	static void Shutdown() noexcept;

	static std::unique_ptr<Renderer> CreateRenderer(const RendererOptions& options_);

private:
	[[nodiscard]] static bool CheckTearingSupport();
	[[nodiscard]] static bool CreateMeshPipeline();
	[[nodiscard]] static bool CreateUIPipeline();
	[[nodiscard]] static ID3D12PipelineState* GetMeshPipelineState(const Material& material_);
	[[nodiscard]] static ID3D12PipelineState* GetUIPipelineState(const Material& material_);
	[[nodiscard]] static bool CreateMeshPipelineState(
		const D3D12_SHADER_BYTECODE& vertexShader_,
		const D3D12_SHADER_BYTECODE& pixelShader_,
		ID3D12PipelineState** pipelineState_);
	[[nodiscard]] static bool CreateUIPipelineState(
		const D3D12_SHADER_BYTECODE& vertexShader_,
		const D3D12_SHADER_BYTECODE& pixelShader_,
		ID3D12PipelineState** pipelineState_);

	static Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	static Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> meshRootSignature;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> meshPipelineState;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> uiRootSignature;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> uiPipelineState;
	static std::unordered_map<std::size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> meshMaterialPipelineStates;
	static std::unordered_map<std::size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> uiMaterialPipelineStates;
	static bool tearingSupported;
};
