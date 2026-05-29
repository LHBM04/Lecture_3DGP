#pragma once
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>

#include "Singleton.h"

class Camera;
class GameObject;

class RenderSystem : public Singleton<RenderSystem>
{
public:
	~RenderSystem() override = default;

	bool Initialize(HWND window_);
	void Release();

	void SetCamera(Camera* camera_);
	void SetObject(GameObject* object_);

	void BeginFrame();
	void EndFrame();

	void Clear();
	void Present();

private:
	struct CameraConstants final
	{
	};

	struct LightConstants final
	{
	};

	struct ObjectConstants final
	{
	};

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

};
