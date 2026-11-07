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
		Asset() noexcept = default;
		virtual ~Asset() noexcept = default;

		explicit Asset(std::filesystem::path sourcePath) noexcept;

		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;

		Asset(Asset&&) noexcept = default;
		Asset& operator=(Asset&&) noexcept = default;

		[[nodiscard]] virtual std::string Serialize() const = 0;
		[[nodiscard]] virtual bool Deserialize(std::string_view text) = 0;

		[[nodiscard]] std::string_view GetName() const noexcept;
		void SetName(std::string_view value) noexcept;

		[[nodiscard]] const std::filesystem::path& GetPath() const noexcept;

	private:
		std::string name;
		std::filesystem::path path;
	};

	inline std::string_view Asset::GetName() const noexcept
	{
		return name;
	}

	inline void Asset::SetName(std::string_view value) noexcept
	{
		name.assign(value);
	}

	inline const std::filesystem::path& Asset::GetPath() const noexcept
	{
		return path;
	}

	inline Asset::Asset(std::filesystem::path sourcePath) noexcept
		: name(sourcePath.stem().string())
		, path(std::move(sourcePath))
	{
	}
}
