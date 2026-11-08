#include "Precompiled.hpp"
#include "GPUShader.hpp"

namespace TUK::Framework
{
	std::expected<GPUShader, std::string> GPUShader::Create(const std::filesystem::path& path)
	{
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

		return GPUShader(std::move(dxcBlob));
	}
}
