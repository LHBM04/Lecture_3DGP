#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Subsystem.h"

namespace TUK::Framework
{
	class Engine
	{
	public:
		Engine() noexcept = default;
		~Engine() noexcept = default;

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

	private:
		std::vector<std::unique_ptr<Subsystem>> subsystems;
		std::unordered_map<std::type_index, Subsystem*> subsystemMap;
	};
}
