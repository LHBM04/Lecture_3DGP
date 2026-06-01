#pragma once

#include <cstddef>

#include "Singleton.h"

class RenderSystem : public Singleton<RenderSystem>
{
public:
	static constexpr std::size_t BufferOffset{ 256 };
	
	struct alignas(BufferOffset) CameraConstants final
	{

	};

	struct alignas(BufferOffset) LightConstants final
	{

	};

	struct alignas(BufferOffset) ObjectConstants final
	{

	};

	struct alignas(BufferOffset) MaterialConstants final
	{

	};

	virtual ~RenderSystem() = default;

	bool Initialize();
	void Release();

private:
};
