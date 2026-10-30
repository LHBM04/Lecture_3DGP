#pragma once

#include "InputState.hpp"

namespace TUK::Framework
{
	class InputProvider
	{
	public:
		InputProvider() noexcept = default;
		virtual ~InputProvider() noexcept = default;

		InputProvider(const InputProvider&) = delete;
		InputProvider& operator=(const InputProvider&) = delete;

		InputProvider(InputProvider&&) = delete;
		InputProvider& operator=(InputProvider&&) = delete;

        inline void Reset()
        {
            lastKeyStates = nowKeyStates;
        }

        inline void OnKeyPressed(int key)
        {
            nowKeyStates[static_cast<std::size_t>(key)] = true;
        }

        inline void OnKeyReleased(int key)
        {
            nowKeyStates[static_cast<std::size_t>(key)] = false;
        }

        [[nodiscard]] inline bool IsKeyPressed(char key) const noexcept
        {
            return nowKeyStates[static_cast<std::size_t>(key)] &&
                  !lastKeyStates[static_cast<std::size_t>(key)];
        }

        /*
         * @brief 지정한 키가 떼어졌는지 여부를 반환합니다.
         *
         * @param key 지정할 키
         *
         * @return bool 키가 떼어졌는지 여부
         */
        [[nodiscard]] inline bool IsKeyReleased(char key) const noexcept
        {
            return !nowKeyStates[static_cast<std::size_t>(key)] &&
                    lastKeyStates[static_cast<std::size_t>(key)];
        }

        /**
         * @brief 지정한 키가 눌려져 있는지 여부를 반환합니다.
         *
         * @param key 지정할 키
         *
         * @return bool 키가 눌려져 있는지 여부
         */
        [[nodiscard]] inline bool IsKeyHeld(char key) const noexcept
        {
            return nowKeyStates[static_cast<std::size_t>(key)] &&
                   lastKeyStates[static_cast<std::size_t>(key)];
        }

	private:
        std::array<bool, 256> lastKeyStates;
        std::array<bool, 256> nowKeyStates;
	};
}
