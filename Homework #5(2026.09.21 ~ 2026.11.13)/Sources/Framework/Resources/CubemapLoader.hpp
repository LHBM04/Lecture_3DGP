#pragma once

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include "AssetLoader.hpp"
#include "Cubemap.hpp"

namespace TUK::Framework
{
	class AssetSubsystem;
	class GPUDevice;

	class CubemapLoader final : public AssetLoader<Cubemap>
	{
	public:
		explicit CubemapLoader(AssetSubsystem* owner, const GPUDevice& device) noexcept;

		[[nodiscard]] std::expected<std::unique_ptr<Cubemap>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		const GPUDevice& device;
	};

	inline CubemapLoader::CubemapLoader(AssetSubsystem* owner, const GPUDevice& device) noexcept
		: AssetLoader<Cubemap>(owner)
		, device(device)
	{
	}
}
