#include "Precompiled.hpp"
#include "ShaderAssetLoader.hpp"

#include <cstddef>
#include <cstring>
#include <format>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wrl/client.h>

#include <directx/d3d12shader.h>
#include <directx/d3dx12.h>
#include <directx-dxc/dxcapi.h>

#include "../Graphics/GPUShader.hpp"

#include "MeshAsset.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		constexpr D3D12_INPUT_ELEMENT_DESC MESH_INPUT_ELEMENTS[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<UINT>(offsetof(MeshVertex, position)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<UINT>(offsetof(MeshVertex, normal)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, static_cast<UINT>(offsetof(MeshVertex, color)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, static_cast<UINT>(offsetof(MeshVertex, uv0)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		void ApplyAlphaBlend(D3D12_BLEND_DESC& blendState) noexcept
		{
			D3D12_RENDER_TARGET_BLEND_DESC& blendTarget = blendState.RenderTarget[0];
			blendTarget.BlendEnable = TRUE;
			blendTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendTarget.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blendTarget.BlendOp = D3D12_BLEND_OP_ADD;
			blendTarget.SrcBlendAlpha = D3D12_BLEND_ONE;
			blendTarget.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			blendTarget.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blendTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}

		void ConfigurePipelineProfile(std::string_view shaderName, ShaderAsset& asset) noexcept
		{
			D3D12_RASTERIZER_DESC rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			rasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			rasterizerState.CullMode = D3D12_CULL_MODE_BACK;
			D3D12_BLEND_DESC blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			D3D12_DEPTH_STENCIL_DESC depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			D3D_PRIMITIVE_TOPOLOGY primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			if (shaderName == "Skybox")
			{
				rasterizerState.CullMode = D3D12_CULL_MODE_NONE;
				depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
				depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			}
			else if (shaderName == "ImageUI" || shaderName == "TextUI")
			{
				rasterizerState.CullMode = D3D12_CULL_MODE_NONE;
				depthStencilState.DepthEnable = FALSE;
				depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				ApplyAlphaBlend(blendState);
				primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			}
			else if (shaderName == "WorldTransition")
			{
				rasterizerState.CullMode = D3D12_CULL_MODE_NONE;
				depthStencilState.DepthEnable = FALSE;
				depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				ApplyAlphaBlend(blendState);
			}

			asset.SetRasterizerState(rasterizerState);
			asset.SetBlendState(blendState);
			asset.SetDepthStencilState(depthStencilState);
			asset.SetPrimitive(primitive);
		}
	}

	std::expected<std::unique_ptr<ShaderAsset>, std::string> ShaderAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".cso")
		{
			return std::unexpected("셰이더 로더에는 .cso 파일이 필요합니다.");
		}

		Microsoft::WRL::ComPtr<IDxcUtils> utils;
		if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))
		{
			return std::unexpected("IDxcUtils 인스턴스 생성에 실패했습니다.");
		}

		Microsoft::WRL::ComPtr<IDxcBlobEncoding> dxcBlob;
		if (FAILED(utils->LoadFile(path.c_str(), nullptr, dxcBlob.GetAddressOf())))
		{
			return std::unexpected("셰이더 파일 로드에 실패했습니다.");
		}

		const DxcBuffer reflectionBuffer
		{
			.Ptr = dxcBlob->GetBufferPointer(),
			.Size = dxcBlob->GetBufferSize(),
			.Encoding = DXC_CP_ACP
		};

		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection;
		if (FAILED(utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&reflection))))
		{
			return std::unexpected("셰이더 리플렉션 생성에 실패했습니다.");
		}

		D3D12_SHADER_DESC shaderDescription{};
		if (FAILED(reflection->GetDesc(&shaderDescription)))
		{
			return std::unexpected("셰이더 리플렉션 정보를 가져오지 못했습니다.");
		}

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
		if (D3D12_SHVER_GET_TYPE(shaderDescription.Version) == D3D12_SHVER_VERTEX_SHADER)
		{
			inputElements.reserve(shaderDescription.InputParameters);
			for (u32 i = 0; i < shaderDescription.InputParameters; ++i)
			{
				D3D12_SIGNATURE_PARAMETER_DESC parameterDescription{};
				if (FAILED(reflection->GetInputParameterDesc(i, &parameterDescription)))
				{
					return std::unexpected("정점 셰이더 입력 파라미터 정보를 가져오지 못했습니다.");
				}

				if (parameterDescription.SystemValueType != D3D_NAME_UNDEFINED)
				{
					continue;
				}

				const D3D12_INPUT_ELEMENT_DESC* matchedElement = nullptr;
				for (const D3D12_INPUT_ELEMENT_DESC& candidate : MESH_INPUT_ELEMENTS)
				{
					if (std::strcmp(candidate.SemanticName, parameterDescription.SemanticName) == 0 &&
						candidate.SemanticIndex == parameterDescription.SemanticIndex)
					{
						matchedElement = &candidate;
						break;
					}
				}

				if (!matchedElement)
				{
					return std::unexpected(std::format(
						"지원하지 않는 정점 입력 시맨틱입니다: {}{}",
						parameterDescription.SemanticName ? parameterDescription.SemanticName : "",
						parameterDescription.SemanticIndex));
				}

				inputElements.push_back(*matchedElement);
			}
		}

		std::filesystem::path shaderNamePath = path.stem();
		if (shaderNamePath.has_extension())
		{
			shaderNamePath = shaderNamePath.stem();
		}

		auto asset = std::make_unique<ShaderAsset>(path, GPUShader(std::move(dxcBlob)));
		asset->SetInputElements(std::move(inputElements));
		ConfigurePipelineProfile(shaderNamePath.string(), *asset);

		return asset;
	}
}
