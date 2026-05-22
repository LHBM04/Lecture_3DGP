#pragma once

class Engine
{
public:
	struct Options final
	{
		const wchar_t* title{ L"New Application" };
		int width{ 800 };
		int height{ 600 };
		bool fullscreen{ false };
		bool resizable{ true };
		bool borderless{ false };
	};

	Engine(HINSTANCE instance_);
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

	bool Initialize(const Options& options_);
	int Run();

private:
	void Shutdown();

	bool CreateMainWindow();

	bool CreateDevice();
	bool CreateCommandQueueAndList();
	bool CreateSwapChain();

	bool CreateRtvAndDsvDescriptorHeaps();

	bool CreateRenderTargetViews();
	bool CreateDepthStencilView();
	bool CreateFence();

	void Render();
	void Resize(int width_, int height_);
	void WaitForGpu();
	void MoveToNextFrame();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
	
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	Options options;

	HINSTANCE instance{ nullptr };
	HWND window{ nullptr };

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController{ nullptr };
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Device> device{ nullptr };

	bool msaa4xEnable{ false };
	UINT msaa4xQualityLevels{ 0 };

	static constexpr UINT BUFFER_COUNT{ 2 };
	UINT currentBufferIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[BUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap{ nullptr };
	UINT rtvDescriptorIncrementSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap{ nullptr };
	UINT dsvDescriptorIncrementSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Fence> fence{ nullptr };
	UINT64 fenceValues[BUFFER_COUNT]{};
	HANDLE fenceEvent{ nullptr };
};
