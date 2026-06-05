#pragma once

#include <array>
#include <memory>

#include <wrl.h>

#include <d3d12.h>

#include "Service.h"

class DescriptorAllocator;
class RenderContext;
class RenderDevice;
class SwapChain;
class UploadBuffer;

class RenderService : public Service
{
public:
	RenderService() noexcept;
	~RenderService() noexcept override;

	[[nodiscard]] bool IsInitialized() const noexcept;

	void BeginFrame();
	void Clear();
	void Present();

protected:
	virtual void OnAdd() override;
	virtual void OnRemove() override;

private:
	static constexpr UINT BackBufferCount{ 2 };
	static constexpr UINT FrameConstantBufferSize{ 64 * 1024 };

	UINT frameIndex{ 0 };
	bool isInitialized{ false };

	std::unique_ptr<RenderDevice> device;
	std::unique_ptr<RenderContext> context;
	std::unique_ptr<SwapChain> swapChain;

	std::unique_ptr<DescriptorAllocator> rtvAllocator;
	std::unique_ptr<DescriptorAllocator> dsvAllocator;
	std::unique_ptr<DescriptorAllocator> srvAllocator;
	std::array<std::unique_ptr<UploadBuffer>, BackBufferCount> uploadBuffers;
};
