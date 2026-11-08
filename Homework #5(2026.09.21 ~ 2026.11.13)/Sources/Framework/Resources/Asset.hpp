#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class Asset
	{
	public:
		explicit Asset(std::filesystem::path path) noexcept;
		virtual ~Asset() noexcept = default;

		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;

		Asset(Asset&&) = delete;
		Asset& operator=(Asset&&) = delete;

		[[nodiscard]] std::string_view GetName() const noexcept;
		[[nodiscard]] const std::filesystem::path& GetPath() const noexcept;

	private:
		std::string name;
		std::filesystem::path path;
	};

	inline Asset::Asset(std::filesystem::path path) noexcept
		: name(path.stem().string())
		, path(std::move(path))
	{
	}

	inline std::string_view Asset::GetName() const noexcept
	{
		return name;
	}

	inline const std::filesystem::path& Asset::GetPath() const noexcept
	{
		return path;
	}
}
