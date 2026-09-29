#include "Precompiled.h"

int main(int, char*[])
{
	WindowOptions options{};
	options.title = L"new title";
	options.positionX = 100;
	options.positionY = 100;
	options.sizeX = 1280;
	options.sizeY = 960;
	options.flags = WindowFlags::Resizable;

	Window* window = AddWindow(options);

	std::println("새 창을 생성했습니다.");
	std::println("창의 타이틀: {}", "new title");
	std::println("창의 위치: x={}, y={}", window->GetPositionX(), window->GetPositionY());
	std::println("창의 크기: width={}, height={}", window->GetSizeX(), window->GetSizeY());
	
	std::cin.get();
}
