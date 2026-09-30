#pragma once

#include "System.h"

namespace TUK::Framework
{
	class Engine : public System
	{
	public:
		Engine() noexcept = default;
		~Engine() noexcept override = default;

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;
	};
}
