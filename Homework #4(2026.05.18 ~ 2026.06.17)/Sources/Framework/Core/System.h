#pragma once

#include <any>
#include <concepts>
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Subsystem.h"

namespace TUK::Framework
{
	/** 지정 타입이 서브시스템인가? */
	template <class TSubsystem>
	concept FromSubsystem = std::derived_from<TSubsystem, Subsystem>;

	class System
	{
	public:
		System();
		~System();

		/** 복사 금지 */
		System(const System&) = delete;
		System& operator=(const System&) = delete;

		/** 이동 금지 */
		System(System&&) = delete;
		System& operator=(System&&) = delete;

		/** 정적 인스턴스 가져오기 */
		[[nodiscard]] static System& GetInstance();

		/** 시스템 실행 */
		int Run();

		/** 종료 요청 */
		void RequestQuit(int code) noexcept;
		[[nodiscard]] bool IsRunning() const noexcept;

		/** 지정한 타입으로 옵션 생성 및 추가 */
		template <class TOption, class TValue>
		std::expected<void, std::string> AddOption(std::string key, TValue&& value);

		/** 옵션 가져오기 */
		template <class TOption>
		[[nodiscard]] std::expected<std::reference_wrapper<TOption>, std::string> GetOption(const std::string& key);

		/** 저장된 타입으로 옵션 가져오기(const) */
		template <class TOption>
		[[nodiscard]] std::expected<std::reference_wrapper<const TOption>, std::string> GetOption(const std::string& key) const;

		/** 서브시스템 추가 */
		template <FromSubsystem TSubsystem>
		std::expected<std::reference_wrapper<TSubsystem>, std::string> AddSubsystem();

		/** 서브시스템 가져오기 */
		template <FromSubsystem TSubsystem>
		[[nodiscard]] std::expected<std::reference_wrapper<TSubsystem>, std::string> GetSubsystem();

		/** 서브시스템 가져오기(const) */
		template <FromSubsystem TSubsystem>
		[[nodiscard]] std::expected<std::reference_wrapper<const TSubsystem>, std::string> GetSubsystem() const;

	private:
		/** 정적 인스턴스 */
		static System* instance;

		/** 시스템 가동 */
		void Startup();

		/** 시스템 종료 */
		void Shutdown();

		/** 실행 여부 */
		bool isRunning;

		/** 종료 코드 */
		int exitCode;

		/** 시스템 설정 */
		std::unordered_map<std::string, std::any> options;

		/** 실질적인 저장은 여기에 */
		std::vector<std::unique_ptr<Subsystem>> subsystems;

		/** 검색용 맵 */
		std::unordered_map<std::type_index, std::reference_wrapper<Subsystem>> subsystemsByType;
	};

	template <class TOption, class TValue>
	std::expected<void, std::string> System::AddOption(std::string key, TValue&& value)
	{
		const auto [iterator, inserted] = options.try_emplace(
			std::move(key),
			std::in_place_type<TOption>,
			std::forward<TValue>(value));

		if (!inserted)
		{
			return std::unexpected(std::string{ "이미 등록된 옵션입니다." });
		}

		return {};
	}

	template <class TOption>
	std::expected<std::reference_wrapper<TOption>, std::string> System::GetOption(const std::string& key)
	{
		const auto result = options.find(key);
		if (result == options.end())
		{
			return std::unexpected("해당 옵션이 등록되어 있지 않습니다: " + key);
		}

		auto* value = std::any_cast<TOption>(&result->second);
		if (!value)
		{
			return std::unexpected("옵션의 타입이 일치하지 않습니다: " + key);
		}

		return std::ref(*value);
	}

	template <class TOption>
	std::expected<std::reference_wrapper<const TOption>, std::string> System::GetOption(const std::string& key) const
	{
		const auto result = options.find(key);
		if (result == options.end())
		{
			return std::unexpected("해당 옵션이 등록되어 있지 않습니다: " + key);
		}

		const auto* value = std::any_cast<TOption>(&result->second);
		if (!value)
		{
			return std::unexpected("옵션의 타입이 일치하지 않습니다: " + key);
		}

		return std::cref(*value);
	}

	template <FromSubsystem TSubsystem>
	std::expected<std::reference_wrapper<TSubsystem>, std::string> System::AddSubsystem()
	{
		const std::type_index type = typeid(TSubsystem);
		if (subsystemsByType.contains(type))
		{
			return std::unexpected(std::string{ "이미 등록된 서브시스템입니다." });
		}

		auto subsystem = std::make_unique<TSubsystem>();
		auto reference = std::ref(*subsystem);

		subsystems.push_back(std::move(subsystem));
		subsystemsByType.emplace(type, reference);

		return reference;
	}

	template <FromSubsystem TSubsystem>
	std::expected<std::reference_wrapper<TSubsystem>, std::string> System::GetSubsystem()
	{
		const auto result = subsystemsByType.find(typeid(TSubsystem));
		if (result == subsystemsByType.end())
		{
			return std::unexpected(std::string{"해당 서브시스템이 등록되어 있지 않습니다."});
		}

		return std::ref(static_cast<TSubsystem&>(result->second.get()));
	}

	template <FromSubsystem TSubsystem>
	std::expected<std::reference_wrapper<const TSubsystem>, std::string> System::GetSubsystem() const
	{
		const auto result = subsystemsByType.find(typeid(TSubsystem));
		if (result == subsystemsByType.end())
		{
			return std::unexpected(std::string{"해당 서브시스템이 등록되어 있지 않습니다."});
		}

		return std::cref(static_cast<const TSubsystem&>(result->second.get()));
	}
}

