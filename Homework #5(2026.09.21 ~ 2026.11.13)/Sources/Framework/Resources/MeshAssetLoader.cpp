#include "Precompiled.hpp"
#include "MeshAssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <span>
#include <string>
#include <utility>

#include <directxtk12/VertexTypes.h>

#include "../Graphics/GPUBuffer.hpp"
#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUMesh.hpp"

namespace TUK::Framework
{
	MeshAssetLoader::MeshAssetLoader(const GPUDevice& device) noexcept
		: device(device)
	{
	}

	std::expected<std::unique_ptr<MeshAsset>, std::string> MeshAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".bin")
		{
			return std::unexpected("메시 로더에는 .bin 메시 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return std::unexpected("메시 파일을 열지 못했습니다: " + path.generic_string());
		}

		const std::string bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
		auto mesh = std::make_unique<MeshAsset>(path);
		if (file.bad() || !mesh->Deserialize(bytes))
		{
			return std::unexpected("메시 역직렬화에 실패했습니다: " + path.generic_string());
		}

		if (mesh->GetName().empty())
		{
			mesh->SetName(path.stem().string());
		}

		auto gpuMesh = device.CreateMesh(
			std::as_bytes(mesh->GetVertices()),
			sizeof(DirectX::VertexPositionNormalColor),
			std::as_bytes(mesh->GetIndices()),
			DXGI_FORMAT_R32_UINT,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		if (!gpuMesh)
		{
			return std::unexpected(gpuMesh.error());
		}

		mesh->SetGPUMesh(std::move(*gpuMesh));
		return mesh;
	}
}
