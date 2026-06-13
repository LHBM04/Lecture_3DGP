#pragma once

#include <vector>
#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Service.h"
#include "Window.h"

class WindowService : public Service
{
public:
	~WindowService() noexcept override = default;

	Window* AddWindow(const Window::Options& options_);
	void RemoveWindow(Window* window);

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	// TODO: 이거 접근 방법 해결하기
	// 이거 이름 뭘로 지어야 하나
	// ::LoadStringW()로 가져와도 될 거 같은데, .rc 파일 생기는 게 너무 싫은데...
	static constexpr LPCWSTR WindowClassName{ L"Framework Window Class" };

	HINSTANCE hInstance;
	std::vector<std::unique_ptr<Window>> windows;
};
