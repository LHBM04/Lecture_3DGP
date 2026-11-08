#include "Precompiled.hpp"
#include "ShaderAssetLoader.hpp"

#include <utility>

#include <wrl/client.h>

#include <directx-dxc/dxcapi.h>

#include "../Graphics/GPUShader.hpp"

#include "ShaderAsset.hpp"

namespace TUK::Framework
{
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

		return std::make_unique<ShaderAsset>(path, GPUShader(std::move(dxcBlob)));
	}
}
