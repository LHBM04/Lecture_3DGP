#pragma once

class ApplicationRequest final
{
public:
	void QuitGame() noexcept;
	[[nodiscard]] bool IsQuitRequested() const noexcept;
	void Clear() noexcept;

private:
	bool quitRequested{ false };
};
