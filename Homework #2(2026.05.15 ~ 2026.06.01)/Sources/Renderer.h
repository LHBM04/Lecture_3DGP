#pragma once

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer operator=(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;
	Renderer operator=(Renderer&&) = delete;

	void BeginRender();
	void EndRender();
};
