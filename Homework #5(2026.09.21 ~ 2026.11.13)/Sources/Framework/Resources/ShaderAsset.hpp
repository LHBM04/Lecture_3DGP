#pragma once

#include <filesystem>
#include <span>
#include <utility>
#include <vector>

#include <directx/d3d12.h>
#include <directx/d3dx12.h>

#include "../Core/Common.hpp"

#include "../Graphics/GPUShader.hpp"
#include "../Graphics/GPUTarget.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class ShaderAsset final : public Asset
	{
	public:
		explicit ShaderAsset(std::filesystem::path path, GPUShader&& shader) noexcept;

		[[nodiscard]] const GPUShader& GetShader() const noexcept;

		[[nodiscard]] std::span<const D3D12_INPUT_ELEMENT_DESC> GetInputElements() const noexcept;
		void SetInputElements(std::vector<D3D12_INPUT_ELEMENT_DESC>&& value) noexcept;

		[[nodiscard]] const D3D12_RASTERIZER_DESC& GetRasterizerState() const noexcept;
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& value) noexcept;

		[[nodiscard]] const D3D12_BLEND_DESC& GetBlendState() const noexcept;
		void SetBlendState(const D3D12_BLEND_DESC& value) noexcept;

		[[nodiscard]] const D3D12_DEPTH_STENCIL_DESC& GetDepthStencilState() const noexcept;
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& value) noexcept;

		[[nodiscard]] D3D_PRIMITIVE_TOPOLOGY GetPrimitive() const noexcept;
		void SetPrimitive(D3D_PRIMITIVE_TOPOLOGY value) noexcept;

		[[nodiscard]] D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePipelineDescription(
			DXGI_FORMAT targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM) const noexcept;

	private:
		GPUShader shader;
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
		D3D12_RASTERIZER_DESC rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		D3D12_BLEND_DESC blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		D3D12_DEPTH_STENCIL_DESC depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		D3D_PRIMITIVE_TOPOLOGY primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};

	inline ShaderAsset::ShaderAsset(std::filesystem::path path, GPUShader&& shader) noexcept
		: Asset(std::move(path))
		, shader(std::move(shader))
	{
	}

	inline const GPUShader& ShaderAsset::GetShader() const noexcept
	{
		return shader;
	}

	inline std::span<const D3D12_INPUT_ELEMENT_DESC> ShaderAsset::GetInputElements() const noexcept
	{
		return inputElements;
	}

	inline void ShaderAsset::SetInputElements(std::vector<D3D12_INPUT_ELEMENT_DESC>&& value) noexcept
	{
		inputElements = std::move(value);
	}

	inline const D3D12_RASTERIZER_DESC& ShaderAsset::GetRasterizerState() const noexcept
	{
		return rasterizerState;
	}

	inline void ShaderAsset::SetRasterizerState(const D3D12_RASTERIZER_DESC& value) noexcept
	{
		rasterizerState = value;
	}

	inline const D3D12_BLEND_DESC& ShaderAsset::GetBlendState() const noexcept
	{
		return blendState;
	}

	inline void ShaderAsset::SetBlendState(const D3D12_BLEND_DESC& value) noexcept
	{
		blendState = value;
	}

	inline const D3D12_DEPTH_STENCIL_DESC& ShaderAsset::GetDepthStencilState() const noexcept
	{
		return depthStencilState;
	}

	inline void ShaderAsset::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& value) noexcept
	{
		depthStencilState = value;
	}

	inline D3D_PRIMITIVE_TOPOLOGY ShaderAsset::GetPrimitive() const noexcept
	{
		return primitive;
	}

	inline void ShaderAsset::SetPrimitive(D3D_PRIMITIVE_TOPOLOGY value) noexcept
	{
		primitive = value;
	}

	inline D3D12_GRAPHICS_PIPELINE_STATE_DESC ShaderAsset::CreatePipelineDescription(DXGI_FORMAT targetFormat) const noexcept
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.InputLayout = {
			inputElements.empty() ? nullptr : inputElements.data(),
			static_cast<UINT>(inputElements.size())
		};
		description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		if (primitive == D3D_PRIMITIVE_TOPOLOGY_POINTLIST)
		{
			description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		}
		else if (primitive == D3D_PRIMITIVE_TOPOLOGY_LINELIST || primitive == D3D_PRIMITIVE_TOPOLOGY_LINESTRIP)
		{
			description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		}
		else if (primitive >= D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST)
		{
			description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		}
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = targetFormat;
		description.SampleMask = UINT_MAX;
		description.SampleDesc = { 1, 0 };
		description.RasterizerState = rasterizerState;
		description.BlendState = blendState;
		description.DepthStencilState = depthStencilState;
		description.DSVFormat = GPUTarget::DEPTH_FORMAT;
		return description;
	}
}
