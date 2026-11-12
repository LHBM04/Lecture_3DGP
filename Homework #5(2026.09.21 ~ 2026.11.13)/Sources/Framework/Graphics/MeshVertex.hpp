#pragma once

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	struct MeshVertex final
	{
		DirectX::XMFLOAT3 position{};
		DirectX::XMFLOAT3 normal{};
		DirectX::XMFLOAT4 color{};
		DirectX::XMFLOAT2 textureCoordinate{};
	};

	inline constexpr std::array<D3D12_INPUT_ELEMENT_DESC, 4> MESH_VERTEX_INPUT_ELEMENTS
	{
		D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<u32>(offsetof(MeshVertex, position)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<u32>(offsetof(MeshVertex, normal)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, static_cast<u32>(offsetof(MeshVertex, color)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, static_cast<u32>(offsetof(MeshVertex, textureCoordinate)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}
