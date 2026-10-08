#pragma once

#include <d3d12.h>
#include <wrl.h>

namespace TUK::Framework
{
	class RenderSubsystem;
	class Renderer;

	class Shader final
	{
		friend class RenderSubsystem;
		friend class Renderer;

	public:
		Shader() noexcept = default;
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader(Shader&&) noexcept = default;
		Shader& operator=(Shader&&) noexcept = default;

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState{};
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature{};
		bool isCompute = false;
	};
}
