#pragma once

class Renderer
{
public:
	struct Options final
	{
		HWND window{ nullptr };
		int width{ 0 };
		int height{ 0 };
		bool vSync{ true };
		bool fullscreen{ false };
		bool enableTripleBuffering{ false };
	};

	Renderer() = default;
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer operator=(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;
	Renderer operator=(Renderer&&) = delete;

	void BeginRender();
	void EndRender();

};
