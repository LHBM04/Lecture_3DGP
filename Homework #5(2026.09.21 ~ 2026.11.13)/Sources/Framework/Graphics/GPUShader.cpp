#include "Precompiled.hpp"
#include "GPUShader.hpp"

#include <format>
#include <iterator>

namespace TUK::Framework
{
	std::expected<GPUShader, std::string> GPUShader::Create(
		const std::filesystem::path& path,
		LPCTSTR target,
		LPCTSTR entryPoint)
	{
		ComPtr<IDxcUtils> utils;
		ComPtr<IDxcCompiler3> compiler;
		ComPtr<IDxcIncludeHandler> includeHandler;
		if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf())))
			|| FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())))
			|| FAILED(utils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf())))
		{
			return std::unexpected("DXC 초기화에 실패했습니다.");
		}

		ComPtr<IDxcBlobEncoding> source;
		HRESULT result = utils->LoadFile(path.c_str(), nullptr, source.GetAddressOf());
		if (FAILED(result))
		{
			return std::unexpected(std::format(
				"셰이더 파일 읽기 실패: {}, HRESULT: 0x{:08X}",
				path.string(),
				static_cast<u32>(result)));
		}

		const std::filesystem::path includeDirectory = path.has_parent_path() ? path.parent_path() : TEXT(".");
		LPCTSTR arguments[] =
		{
			path.c_str(), TEXT("-E"), entryPoint, TEXT("-T"), target,
			TEXT("-I"), includeDirectory.c_str(), TEXT("-Ges"), TEXT("-WX"), TEXT("-HV"), TEXT("2018")
		};

		BOOL knownEncoding = FALSE;
		UINT32 codePage = DXC_CP_UTF8;
		source->GetEncoding(&knownEncoding, &codePage);
		const DxcBuffer SOURCE_BUFFER
		{
			source->GetBufferPointer(),
			source->GetBufferSize(),
			knownEncoding ? codePage : DXC_CP_UTF8
		};

		ComPtr<IDxcResult> compilation;
		result = compiler->Compile(
			&SOURCE_BUFFER,
			arguments,
			static_cast<UINT32>(std::size(arguments)),
			includeHandler.Get(),
			IID_PPV_ARGS(compilation.GetAddressOf()));
		if (FAILED(result))
		{
			return std::unexpected(std::format("DXC 호출 실패: 0x{:08X}", static_cast<u32>(result)));
		}

		HRESULT status = E_FAIL;
		result = compilation->GetStatus(&status);
		if (FAILED(result) || FAILED(status))
		{
			std::string message = std::format(
				"GPUShader 컴파일 실패: {}, HRESULT: 0x{:08X}",
				path.string(),
				static_cast<u32>(FAILED(result) ? result : status));
			ComPtr<IDxcBlobUtf8> errors;
			if (SUCCEEDED(compilation->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr))
				&& errors && errors->GetStringLength() > 0)
			{
				message += '\n';
				message.append(errors->GetStringPointer(), errors->GetStringLength());
			}

			return std::unexpected(std::move(message));
		}

		ComPtr<IDxcBlob> object;
		result = compilation->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(object.GetAddressOf()), nullptr);
		if (FAILED(result) || !object || object->GetBufferSize() == 0)
		{
			return std::unexpected("DXC 셰이더 바이트코드를 가져오지 못했습니다.");
		}

		return GPUShader(std::move(object));
	}
}
