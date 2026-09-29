#include <iostream>
#include <print>

#include "Framework/Platform/Window.h"

int main(int, char*[])
{
	TUK::Framework::Window* window = TUK::Framework::AddWindow();

	std::println("새 창을 생성했습니다.");
	std::println("창의 타이틀: {}", "new title");
	std::println("창의 위치: x={}, y={}", window->GetPositionX(), window->GetPositionY());
	std::println("창의 크기: width={}, height={}", window->GetSizeX(), window->GetSizeY());
	
	std::cin.get();
}
