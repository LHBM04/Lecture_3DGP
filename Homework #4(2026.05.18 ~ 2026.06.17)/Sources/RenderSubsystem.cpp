#include "Precompiled.h"
#include "RenderSubsystem.h"

#include "Logger.h"

namespace
{
	/** D3D12 디바이스 */
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	std::expected<void, std::string> InitializeDevice()
	{
#ifdef _DEBUG

#endif

		return {};
	}
}

std::expected<void, std::string> RenderSubsystem::Startup()
{
	if (auto result = InitializeDevice(); !result)
	{
		return std::unexpected(result.error());
	}
}

void RenderSubsystem::Shutdown() noexcept
{
	LOGTRACE("RenderSubsystem::Shutdown() 호출됨");
}
