#include "Precompiled.hpp"
#include "ShaderAssetImporter.hpp"

#include <wrl/client.h>

#include <directx/d3d12shader.h>

#include "../Graphics/GPUShader.hpp"
#include "../Graphics/MeshVertex.hpp"

#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	namespace
	{
		constexpr bool IsAsciiWhitespace(char ch) noexcept
		{
			return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
		}

		std::string_view Trim(std::string_view text) noexcept
		{
			constexpr std::string_view UTF8_BOM = "\xEF\xBB\xBF";
			if (text.starts_with(UTF8_BOM))
			{
				text.remove_prefix(UTF8_BOM.size());
			}
			while (!text.empty() && IsAsciiWhitespace(text.front()))
			{
				text.remove_prefix(1);
			}
			while (!text.empty() && IsAsciiWhitespace(text.back()))
			{
				text.remove_suffix(1);
			}
			return text;
		}

		template <class TValue, usize N>
		bool TryMapValue(
			std::string_view name,
			const std::array<std::pair<std::string_view, TValue>, N>& table,
			TValue& out) noexcept
		{
			for (const auto& [key, value] : table)
			{
				if (key == name)
				{
					out = value;
					return true;
				}
			}
			return false;
		}

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

		void ApplyMetadataEntry(
			std::string_view key,
			std::string_view value,
			D3D12_RASTERIZER_DESC& rasterizerState,
			D3D12_BLEND_DESC& blendState,
			D3D12_DEPTH_STENCIL_DESC& depthStencilState,
			D3D_PRIMITIVE_TOPOLOGY& primitive) noexcept
		{
			static constexpr std::array<std::pair<std::string_view, D3D12_CULL_MODE>, 3> CULL_MODES
			{ {
				{ "Back", D3D12_CULL_MODE_BACK },
				{ "Front", D3D12_CULL_MODE_FRONT },
				{ "None", D3D12_CULL_MODE_NONE }
			} };
			static constexpr std::array<std::pair<std::string_view, D3D12_FILL_MODE>, 2> FILL_MODES
			{ {
				{ "Solid", D3D12_FILL_MODE_SOLID },
				{ "Wireframe", D3D12_FILL_MODE_WIREFRAME }
			} };
			static constexpr std::array<std::pair<std::string_view, D3D12_COMPARISON_FUNC>, 8> DEPTH_FUNCS
			{ {
				{ "Never", D3D12_COMPARISON_FUNC_NEVER },
				{ "Less", D3D12_COMPARISON_FUNC_LESS },
				{ "Equal", D3D12_COMPARISON_FUNC_EQUAL },
				{ "LessEqual", D3D12_COMPARISON_FUNC_LESS_EQUAL },
				{ "Greater", D3D12_COMPARISON_FUNC_GREATER },
				{ "NotEqual", D3D12_COMPARISON_FUNC_NOT_EQUAL },
				{ "GreaterEqual", D3D12_COMPARISON_FUNC_GREATER_EQUAL },
				{ "Always", D3D12_COMPARISON_FUNC_ALWAYS }
			} };
			static constexpr std::array<std::pair<std::string_view, D3D_PRIMITIVE_TOPOLOGY>, 5> TOPOLOGIES
			{ {
				{ "TriangleList", D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
				{ "TriangleStrip", D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
				{ "LineList", D3D_PRIMITIVE_TOPOLOGY_LINELIST },
				{ "LineStrip", D3D_PRIMITIVE_TOPOLOGY_LINESTRIP },
				{ "PointList", D3D_PRIMITIVE_TOPOLOGY_POINTLIST }
			} };

			if (key == "cull")
			{
				TryMapValue(value, CULL_MODES, rasterizerState.CullMode);
			}
			else if (key == "fill")
			{
				TryMapValue(value, FILL_MODES, rasterizerState.FillMode);
			}
			else if (key == "depth")
			{
				if (value == "Off")
				{
					depthStencilState.DepthEnable = FALSE;
				}
				else if (value == "On")
				{
					depthStencilState.DepthEnable = TRUE;
					depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
				}
				else if (TryMapValue(value, DEPTH_FUNCS, depthStencilState.DepthFunc))
				{
					depthStencilState.DepthEnable = TRUE;
				}
			}
			else if (key == "depthWrite")
			{
				if (value == "Off")
				{
					depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				}
				else if (value == "On")
				{
					depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				}
			}
			else if (key == "blend")
			{
				if (value == "Alpha")
				{
					ApplyAlphaBlend(blendState);
				}
				else if (value == "Opaque")
				{
					blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
				}
			}
			else if (key == "topology")
			{
				TryMapValue(value, TOPOLOGIES, primitive);
			}
		}

		void ApplyShaderMetadata(const std::filesystem::path& csoPath, ShaderAsset& asset) noexcept
		{
			D3D12_RASTERIZER_DESC rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			rasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			rasterizerState.CullMode = D3D12_CULL_MODE_BACK;
			D3D12_BLEND_DESC blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			D3D12_DEPTH_STENCIL_DESC depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			D3D_PRIMITIVE_TOPOLOGY primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			std::filesystem::path shaderNamePath = csoPath.stem();
			if (shaderNamePath.has_extension())
			{
				shaderNamePath = shaderNamePath.stem();
			}

			const std::filesystem::path metaPath = csoPath.parent_path() / (shaderNamePath.string() + ".meta");
			std::ifstream metaFile(metaPath);
			if (metaFile)
			{
				std::string line;
				while (std::getline(metaFile, line))
				{
					const std::string_view trimmed = Trim(line);
					const usize separator = trimmed.find('=');
					if (separator == std::string_view::npos)
					{
						continue;
					}

					const std::string_view key = Trim(trimmed.substr(0, separator));
					const std::string_view value = Trim(trimmed.substr(separator + 1));
					ApplyMetadataEntry(key, value, rasterizerState, blendState, depthStencilState, primitive);
				}
			}

			asset.SetRasterizerState(rasterizerState);
			asset.SetBlendState(blendState);
			asset.SetDepthStencilState(depthStencilState);
			asset.SetPrimitive(primitive);
		}
	}

	std::expected<std::unique_ptr<ShaderAsset>, std::string> ShaderAssetImporter::Import(
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
				for (const D3D12_INPUT_ELEMENT_DESC& candidate : MESH_VERTEX_INPUT_ELEMENTS)
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

		auto asset = std::make_unique<ShaderAsset>(path, GPUShader(std::move(dxcBlob)));
		asset->SetInputElements(std::move(inputElements));
		ApplyShaderMetadata(path, *asset);

		return asset;
	}
}
