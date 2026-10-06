#pragma once

#include <d3d12.h>
#include <format>
#include <string_view>

#include <expected>
#include <string>

namespace TUK::Framework
{
	[[nodiscard]] inline std::expected<void, std::string> CheckHResult(HRESULT result, std::string_view operation)
	{
		if (FAILED(result))
		{
			return std::unexpected(std::format("{} 실패 (HRESULT: 0x{:08X}).",
				operation, static_cast<unsigned long>(result)));
		}
		return {};
	}
}
