#include "Precompiled.hpp"
#include "GPUShader.hpp"

namespace TUK::Framework
{
	std::expected<GPUShader, std::string> GPUShader::Create(const std::filesystem::path& path)
	{
		ComPtr<IDxcUtils> utils;
		if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))
		{
			return std::unexpected("Failed to create IDxcUtils instance.");
		}

		ComPtr<IDxcBlobEncoding> dxcBlob;
		if (FAILED(utils->LoadFile(path.c_str(), nullptr, dxcBlob.GetAddressOf())))
		{
			return std::unexpected("Failed to load shader file.");
		}

		return GPUShader(std::move(dxcBlob));
	}
}
