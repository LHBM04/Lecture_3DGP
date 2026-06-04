#pragma once

class Game
{
public:
	Game() = default;
	~Game() = default;

	void Initialize();

	void Update();
	void Render();

private:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	Game(Game&&) = delete;
	Game& operator=(Game&&) = delete;
};