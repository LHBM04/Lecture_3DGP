#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include <stdexcept>
#include "../Core/Engine.hpp"
#include "../Platform/WindowSubsystem.hpp"
#include "GraphicsError.hpp"

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept : EngineSubsystem(10) {}
	RenderSubsystem::~RenderSubsystem() noexcept { OnShutdown(); }
	Renderer& RenderSubsystem::GetRenderer() noexcept { return renderer; }
	bool RenderSubsystem::IsFrameActive() const noexcept { return renderer.IsRecording() && !hasFailed; }
	ID3D12Device& RenderSubsystem::GetNativeDevice() const noexcept { assert(device); return *device.Get(); }
	RenderTarget& RenderSubsystem::GetWindowTarget(Window& window)
	{
		if (!IsFrameActive()) throw std::logic_error("활성 렌더링 프레임이 아닙니다.");
		for (auto* output : frameOutputs)
			if (!window.ShouldClose() && output->handle == window.GetHWND())
				return output->targets[output->swapChain->GetCurrentBackBufferIndex()];
		throw std::logic_error("현재 프레임에서 출력할 수 없는 창입니다.");
	}
	void RenderSubsystem::BlitToWindow(RenderTarget& source, Window& window)
	{
		renderer.SetRenderTarget(GetWindowTarget(window));
		renderer.Blit(source.GetTexture(), screenShader);
	}
	void RenderSubsystem::ReportError(std::string_view message)
	{
		hasFailed = true;
		Engine::GetInstance().ReportError(message);
	}
	bool RenderSubsystem::CheckResult(HRESULT result, std::string_view operation)
	{
		return CheckResult(CheckHResult(result, operation));
	}
	std::expected<void, std::string> RenderSubsystem::InitializeDevice()
	{
		if (factory || device)
		{
			return std::unexpected(std::string("RenderSubsystem이 이미 초기화되어 있습니다."));
		}
		// 실패한 초기화에서 일부 자원을 멤버에 남기지 않는다.
		Microsoft::WRL::ComPtr<IDXGIFactory6> createdFactory{};
		Microsoft::WRL::ComPtr<ID3D12Device> createdDevice{};
		UINT factoryFlags = 0;
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debug{};
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()))))
		{
			debug->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		const auto factoryResult = CheckHResult(CreateDXGIFactory2(factoryFlags,
			IID_PPV_ARGS(createdFactory.GetAddressOf())), "DXGI 팩토리 생성");
		if (!factoryResult)
		{
			return factoryResult;
		}
		for (UINT index = 0; ; ++index)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter{};
			const HRESULT result = createdFactory->EnumAdapterByGpuPreference(index,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf()));
			if (result == DXGI_ERROR_NOT_FOUND)
			{
				break;
			}
			const auto adapterResult = CheckHResult(result, "어댑터 조회");
			if (!adapterResult)
			{
				return adapterResult;
			}
			DXGI_ADAPTER_DESC1 description{};
			const auto descriptionResult = CheckHResult(adapter->GetDesc1(&description), "어댑터 정보 조회");
			if (!descriptionResult)
			{
				return descriptionResult;
			}
			if (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(createdDevice.ReleaseAndGetAddressOf()))))
			{
				break;
			}
		}
		if (!createdDevice)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> warp{};
			const auto warpResult = CheckHResult(createdFactory->EnumWarpAdapter(
				IID_PPV_ARGS(warp.GetAddressOf())), "WARP 어댑터 조회");
			if (!warpResult)
			{
				return warpResult;
			}
			const auto deviceResult = CheckHResult(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(createdDevice.ReleaseAndGetAddressOf())), "D3D12 디바이스 생성");
			if (!deviceResult)
			{
				return deviceResult;
			}
		}
		factory = std::move(createdFactory);
		device = std::move(createdDevice);
		return {};
	}
	void RenderSubsystem::OnStartup()
	{
		if (isInitialized) return;
		hasFailed = false;
		windowSubsystem = Engine::GetInstance().GetSubsystem<WindowSubsystem>();
		if (!windowSubsystem) { ReportError("WindowSubsystem이 필요합니다."); return; }
		if (!CheckResult(InitializeDevice())) return;
		D3D12_COMMAND_QUEUE_DESC queueDescription{};
		queueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (!CheckResult(device->CreateCommandQueue(&queueDescription, IID_PPV_ARGS(commandQueue.GetAddressOf())), "명령 큐 생성")) return;
		if (!CheckResult(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())), "펜스 생성")) return;
		completionEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!completionEvent) { CheckResult(HRESULT_FROM_WIN32(GetLastError()), "GPU 완료 이벤트 생성"); return; }
		if (!CheckResult(renderer.Initialize(*device.Get()))) return;
		if (!CheckResult(InitializeScreenShader())) return;
		isInitialized = true;
	}
	std::expected<std::unique_ptr<SwapChain>, std::string>
	RenderSubsystem::CreateSwapChain(Window& window, UINT width, UINT height)
	{
		const int bufferCount = Engine::GetInstance().GetOption<int>("Render.BufferCount");
		if (bufferCount < 2 || bufferCount > 16)
			return std::unexpected(std::string("Render.BufferCount는 2 이상 16 이하여야 합니다."));
		auto output = std::make_unique<SwapChain>();
		output->handle = window.GetHWND();
		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = width;
		description.Height = height;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.SampleDesc.Count = 1;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.BufferCount = static_cast<UINT>(bufferCount);
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		const bool fullscreen = window.IsFullscreen() && !window.IsBorderless();
		description.Flags = fullscreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> created{};
		if (auto result = CheckHResult(factory->CreateSwapChainForHwnd(commandQueue.Get(), output->handle,
			&description, nullptr, nullptr, created.GetAddressOf()), "스왑 체인 생성"); !result)
			return std::unexpected(result.error());
		if (auto result = CheckHResult(created.As(&output->swapChain), "스왑 체인 인터페이스 조회"); !result)
			return std::unexpected(result.error());
		if (auto result = CheckHResult(factory->MakeWindowAssociation(output->handle, DXGI_MWA_NO_ALT_ENTER), "창 연결 설정"); !result)
			return std::unexpected(result.error());
		if (fullscreen)
		{
			Microsoft::WRL::ComPtr<IDXGIOutput> display{};
			if (auto result = CheckHResult(output->swapChain->GetContainingOutput(display.GetAddressOf()), "전체 화면 출력 조회"); !result)
				return std::unexpected(result.error());
			const HRESULT result = output->swapChain->SetFullscreenState(TRUE, display.Get());
			if (result != S_OK) return std::unexpected(std::format("전체 화면 전환 실패: 0x{:08X}", static_cast<unsigned long>(result)));
			DXGI_MODE_DESC mode{};
			mode.Width = width;
			mode.Height = height;
			mode.Format = description.Format;
			if (auto resize = CheckHResult(output->swapChain->ResizeTarget(&mode), "전체 화면 해상도 변경"); !resize)
				return std::unexpected(resize.error());
			if (auto resize = CheckHResult(output->swapChain->ResizeBuffers(description.BufferCount, 0, 0,
				description.Format, description.Flags), "전체 화면 백 버퍼 갱신"); !resize)
				return std::unexpected(resize.error());
		}
		if (auto result = UpdateWindowTargets(*output); !result) return std::unexpected(result.error());
		return output;
	}
	std::expected<void, std::string> RenderSubsystem::UpdateWindowTargets(SwapChain& output)
	{
		DXGI_SWAP_CHAIN_DESC1 description{};
		if (auto result = CheckHResult(output.swapChain->GetDesc1(&description), "스왑 체인 설정 조회"); !result) return result;
		output.targets.clear();
		output.targets.resize(description.BufferCount);
		for (UINT index = 0; index < description.BufferCount; ++index)
		{
			auto& target = output.targets[index];
			target.isWindowTarget = true;
			if (auto result = CheckHResult(output.swapChain->GetBuffer(index, IID_PPV_ARGS(target.color.resource.GetAddressOf())), "백 버퍼 조회"); !result) return result;
			target.color.width = description.Width;
			target.color.height = description.Height;
			target.color.format = description.Format;
			target.color.state = D3D12_RESOURCE_STATE_PRESENT;
			D3D12_DESCRIPTOR_HEAP_DESC heap{};
			heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heap.NumDescriptors = 1;
			if (auto result = CheckHResult(device->CreateDescriptorHeap(&heap, IID_PPV_ARGS(target.renderViewHeap.GetAddressOf())), "백 버퍼 RTV 힙 생성"); !result) return result;
			device->CreateRenderTargetView(target.color.resource.Get(), nullptr, target.renderViewHeap->GetCPUDescriptorHandleForHeapStart());
		}
		return {};
	}
	std::expected<void, std::string> RenderSubsystem::UpdateWindows()
	{
		frameOutputs.clear();
		const auto& windows = windowSubsystem->GetWindows();
		std::erase_if(outputs, [&windows](const auto& output)
		{
			return std::ranges::none_of(windows, [&output](const auto& window)
			{ return !window->ShouldClose() && window->GetHWND() == output->handle; });
		});
		for (const auto& window : windows)
		{
			const HWND handle = window->GetHWND();
			if (window->ShouldClose() || !handle || IsIconic(handle) || !IsWindowVisible(handle)) continue;
			RECT client{};
			if (!GetClientRect(handle, &client)) return CheckHResult(HRESULT_FROM_WIN32(GetLastError()), "창 크기 조회");
			const UINT width = static_cast<UINT>(client.right - client.left);
			const UINT height = static_cast<UINT>(client.bottom - client.top);
			if (width == 0 || height == 0) continue;
			auto iterator = std::ranges::find_if(outputs, [handle](const auto& output) { return output->handle == handle; });
			if (iterator == outputs.end())
			{
				auto created = CreateSwapChain(*window, width, height);
				if (!created) return std::unexpected(created.error());
				outputs.push_back(std::move(*created));
				iterator = std::prev(outputs.end());
			}
			auto& output = **iterator;
			if (output.targets.front().GetWidth() != width || output.targets.front().GetHeight() != height)
			{
				DXGI_SWAP_CHAIN_DESC1 description{};
				if (auto result = CheckHResult(output.swapChain->GetDesc1(&description), "스왑 체인 설정 조회"); !result) return result;
				// 이전 프레임의 GPU 완료 대기 후에만 백 버퍼 참조를 해제한다.
				output.targets.clear();
				if (auto result = CheckHResult(output.swapChain->ResizeBuffers(description.BufferCount, width, height,
					description.Format, description.Flags), "백 버퍼 크기 변경"); !result) return result;
				if (auto result = UpdateWindowTargets(output); !result) return result;
			}
			frameOutputs.push_back(&output);
		}
		return {};
	}
	void RenderSubsystem::OnPreTick()
	{
		if (!isInitialized || hasFailed) return;
		if (!CheckResult(UpdateWindows()) || !CheckResult(renderer.Begin())) return;
		const ColorRGBA<float> background(0.08f, 0.12f, 0.18f, 1.0f);
		for (auto* output : frameOutputs)
		{
			renderer.SetRenderTarget(output->targets[output->swapChain->GetCurrentBackBufferIndex()]);
			renderer.Clear(background);
		}
		renderer.currentTarget = nullptr;
	}
	void RenderSubsystem::OnPostTick()
	{
		if (!isInitialized || !renderer.IsRecording()) return;
		for (auto* output : frameOutputs)
			renderer.Transition(output->targets[output->swapChain->GetCurrentBackBufferIndex()].color, D3D12_RESOURCE_STATE_PRESENT);
		if (!CheckResult(renderer.End())) return;
		ID3D12CommandList* lists[]{ renderer.commandList.Get() };
		commandQueue->ExecuteCommandLists(1, lists);
		const auto& windows = windowSubsystem->GetWindows();
		for (auto* output : frameOutputs)
		{
			const bool alive = std::ranges::any_of(windows, [output](const auto& window)
			{ return !window->ShouldClose() && window->GetHWND() == output->handle; });
			if (alive && !CheckResult(output->swapChain->Present(1, 0), "프레임 표시")) break;
		}
		// 실패한 Present 뒤에도 제출된 GPU 작업을 완료시킨다.
		WaitForGpu();
		frameOutputs.clear();
	}
	bool RenderSubsystem::WaitForGpu()
	{
		if (!device || !commandQueue || !fence || !completionEvent) return false;
		if (!CheckResult(device->GetDeviceRemovedReason(), "디바이스 상태 확인")) return false;
		if (fenceValue >= (std::numeric_limits<UINT64>::max)() - 1)
		{ ReportError("GPU 완료 값이 범위를 초과했습니다."); return false; }
		const UINT64 nextValue = fenceValue + 1;
		if (!CheckResult(commandQueue->Signal(fence.Get(), nextValue), "GPU 완료 신호")) return false;
		fenceValue = nextValue;
		const UINT64 completed = fence->GetCompletedValue();
		if (completed == (std::numeric_limits<UINT64>::max)())
		{ ReportError("GPU 완료 조회 중 디바이스가 제거되었습니다."); return false; }
		if (completed < fenceValue)
		{
			if (!CheckResult(fence->SetEventOnCompletion(fenceValue, completionEvent), "GPU 완료 이벤트 설정")) return false;
			const DWORD result = WaitForSingleObject(completionEvent, INFINITE);
			if (result == WAIT_FAILED) { CheckResult(HRESULT_FROM_WIN32(GetLastError()), "GPU 완료 대기"); return false; }
			if (result != WAIT_OBJECT_0) { ReportError("GPU 완료 이벤트 대기에 실패했습니다."); return false; }
		}
		return CheckResult(device->GetDeviceRemovedReason(), "디바이스 상태 확인");
	}
	void RenderSubsystem::OnShutdown()
	{
		if (isInitialized && device && SUCCEEDED(device->GetDeviceRemovedReason())) WaitForGpu();
		frameOutputs.clear();
		outputs.clear();
		renderer.Release();
		screenShader = Shader{};
		if (completionEvent) { CloseHandle(completionEvent); completionEvent = nullptr; }
		fence.Reset();
		commandQueue.Reset();
		device.Reset();
		factory.Reset();
		fenceValue = 0;
		windowSubsystem = nullptr;
		isInitialized = false;
	}
	std::expected<Microsoft::WRL::ComPtr<ID3D12Resource>, std::string> RenderSubsystem::CreateResource(
		const D3D12_RESOURCE_DESC& description, D3D12_HEAP_TYPE heapType,
		D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue) const
	{
		if (!device) return std::unexpected(std::string("디바이스가 초기화되지 않았습니다."));
		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = heapType;
		heap.CreationNodeMask = 1;
		heap.VisibleNodeMask = 1;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		const auto result = CheckHResult(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE,
			&description, initialState, clearValue, IID_PPV_ARGS(resource.GetAddressOf())), "리소스 생성");
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return resource;
	}

	std::expected<Buffer, std::string> RenderSubsystem::CreateBuffer(
		UINT64 size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS flags) const
	{
		if (size == 0) return std::unexpected(std::string("버퍼 크기는 0보다 커야 합니다."));
		D3D12_RESOURCE_DESC description{};
		description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		description.Width = size;
		description.Height = 1;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.SampleDesc.Count = 1;
		description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		description.Flags = flags;
		return CreateResource(description, heapType, initialState, nullptr).transform(
			[size, heapType](auto resource)
			{
				return Buffer(std::move(resource), size, heapType);
			});
	}

	std::expected<Microsoft::WRL::ComPtr<ID3D12RootSignature>, std::string> RenderSubsystem::CreateRootSignature(
		std::span<const std::byte> serializedSignature) const
	{
		if (!device || serializedSignature.empty()) return std::unexpected(std::string("디바이스와 직렬화된 루트 시그니처가 필요합니다."));
		Microsoft::WRL::ComPtr<ID3D12RootSignature> signature{};
		const auto result = CheckHResult(device->CreateRootSignature(0, serializedSignature.data(),
			serializedSignature.size(), IID_PPV_ARGS(signature.GetAddressOf())), "루트 시그니처 생성");
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return signature;
	}
	std::expected<void, std::string> RenderSubsystem::CreateShaderView(Texture& texture) const
	{
		D3D12_DESCRIPTOR_HEAP_DESC heap{};
		heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heap.NumDescriptors = 1;
		heap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (auto result = CheckHResult(device->CreateDescriptorHeap(&heap,
			IID_PPV_ARGS(texture.shaderViewHeap.GetAddressOf())), "텍스처 SRV 힙 생성"); !result) return result;
		device->CreateShaderResourceView(texture.resource.Get(), nullptr, texture.shaderViewHeap->GetCPUDescriptorHandleForHeapStart());
		return {};
	}

	std::expected<Texture, std::string> RenderSubsystem::CreateTexture(UINT width, UINT height, DXGI_FORMAT format) const
	{
		if (!device || width == 0 || height == 0 || width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION || height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)
			return std::unexpected(std::string("텍스처 생성에는 디바이스와 유효한 크기가 필요합니다."));
		D3D12_FEATURE_DATA_FORMAT_SUPPORT support{};
		support.Format = format;
		if (auto result = CheckHResult(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support)), "텍스처 포맷 확인"); !result)
			return std::unexpected(result.error());
		if (!(support.Support1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D) || !(support.Support1 & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE))
			return std::unexpected(std::string("샘플링 가능한 2D 텍스처 포맷이 필요합니다."));
		D3D12_RESOURCE_DESC description{};
		description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		description.Width = width;
		description.Height = height;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.Format = format;
		description.SampleDesc.Count = 1;
		auto resource = CreateResource(description, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		if (!resource) return std::unexpected(resource.error());
		Texture texture{};
		texture.resource = std::move(*resource);
		texture.width = width;
		texture.height = height;
		texture.format = format;
		if (auto result = CreateShaderView(texture); !result) return std::unexpected(result.error());
		return texture;
	}

	std::expected<RenderTarget, std::string> RenderSubsystem::CreateRenderTarget(UINT width, UINT height, bool withDepth, DXGI_FORMAT format) const
	{
		if (!device || width == 0 || height == 0 || width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION || height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)
			return std::unexpected(std::string("렌더 타겟 생성에는 디바이스와 유효한 크기가 필요합니다."));
		D3D12_FEATURE_DATA_FORMAT_SUPPORT support{};
		support.Format = format;
		if (auto result = CheckHResult(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support)), "렌더 타겟 포맷 확인"); !result)
			return std::unexpected(result.error());
		if (!(support.Support1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) || !(support.Support1 & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE))
			return std::unexpected(std::string("그리기 및 샘플링 가능한 컬러 포맷이 필요합니다."));
		D3D12_RESOURCE_DESC description{};
		description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		description.Width = width;
		description.Height = height;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.Format = format;
		description.SampleDesc.Count = 1;
		description.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		D3D12_CLEAR_VALUE clear{};
		clear.Format = format;
		auto resource = CreateResource(description, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, &clear);
		if (!resource) return std::unexpected(resource.error());
		RenderTarget target{};
		target.color.resource = std::move(*resource);
		target.color.width = width;
		target.color.height = height;
		target.color.format = format;
		if (auto result = CreateShaderView(target.color); !result) return std::unexpected(result.error());
		D3D12_DESCRIPTOR_HEAP_DESC heap{};
		heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heap.NumDescriptors = 1;
		if (auto result = CheckHResult(device->CreateDescriptorHeap(&heap, IID_PPV_ARGS(target.renderViewHeap.GetAddressOf())), "렌더 타겟 RTV 힙 생성"); !result)
			return std::unexpected(result.error());
		device->CreateRenderTargetView(target.color.resource.Get(), nullptr, target.renderViewHeap->GetCPUDescriptorHandleForHeapStart());
		if (withDepth)
		{
			description.Format = DXGI_FORMAT_D32_FLOAT;
			description.Flags = static_cast<D3D12_RESOURCE_FLAGS>(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);
			clear.Format = description.Format;
			clear.DepthStencil.Depth = 1.0f;
			clear.DepthStencil.Stencil = 0;
			auto depth = CreateResource(description, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear);
			if (!depth) return std::unexpected(depth.error());
			target.depth.emplace();
			target.depth->resource = std::move(*depth);
			target.depth->width = width;
			target.depth->height = height;
			target.depth->format = description.Format;
			target.depth->state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			if (auto result = CheckHResult(device->CreateDescriptorHeap(&heap, IID_PPV_ARGS(target.depthViewHeap.GetAddressOf())), "깊이 버퍼 DSV 힙 생성"); !result)
				return std::unexpected(result.error());
			device->CreateDepthStencilView(target.depth->resource.Get(), nullptr, target.depthViewHeap->GetCPUDescriptorHandleForHeapStart());
		}
		return target;
	}

	std::expected<Shader, std::string> RenderSubsystem::CreateShader(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const
	{
		if (!device || !description.pRootSignature) return std::unexpected(std::string("디바이스와 루트 시그니처가 필요합니다."));
		Shader shader{};
		auto result = CheckHResult(device->CreateGraphicsPipelineState(&description, IID_PPV_ARGS(shader.pipelineState.GetAddressOf())), "그래픽 셰이더 PSO 생성");
		if (!result) return std::unexpected(result.error());
		shader.rootSignature = description.pRootSignature;
		return shader;
	}

	std::expected<Shader, std::string> RenderSubsystem::CreateComputeShader(const D3D12_COMPUTE_PIPELINE_STATE_DESC& description) const
	{
		if (!device || !description.pRootSignature) return std::unexpected(std::string("디바이스와 루트 시그니처가 필요합니다."));
		Shader shader{};
		auto result = CheckHResult(device->CreateComputePipelineState(&description, IID_PPV_ARGS(shader.pipelineState.GetAddressOf())), "컴퓨트 셰이더 PSO 생성");
		if (!result) return std::unexpected(result.error());
		shader.rootSignature = description.pRootSignature;
		shader.isCompute = true;
		return shader;
	}

	std::expected<void, std::string> RenderSubsystem::InitializeScreenShader()
	{
		constexpr char source[] = R"(
Texture2D image : register(t0);
SamplerState imageSampler : register(s0);
struct VertexOutput { float4 position : SV_Position; float2 uv : TEXCOORD0; };
VertexOutput VS(uint id : SV_VertexID)
{
    VertexOutput output;
    output.uv = float2((id << 1) & 2, id & 2);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}
float4 PS(VertexOutput input) : SV_Target { return image.Sample(imageSampler, input.uv); }
)";
		Microsoft::WRL::ComPtr<ID3DBlob> vertex{};
		Microsoft::WRL::ComPtr<ID3DBlob> pixel{};
		Microsoft::WRL::ComPtr<ID3DBlob> errors{};
		for (const auto stage : { std::pair{ "VS", "vs_5_0" }, std::pair{ "PS", "ps_5_0" } })
		{
			auto& bytecode = std::string_view(stage.first) == "VS" ? vertex : pixel;
			const HRESULT result = D3DCompile(source, sizeof(source) - 1, "ScreenBlit", nullptr, nullptr,
				stage.first, stage.second, D3DCOMPILE_ENABLE_STRICTNESS, 0, bytecode.GetAddressOf(), errors.ReleaseAndGetAddressOf());
			if (FAILED(result)) return std::unexpected(errors ? std::string(static_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize()) : std::string("화면 출력 셰이더 컴파일 실패"));
		}
		D3D12_DESCRIPTOR_RANGE range{};
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.NumDescriptors = 1;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		D3D12_ROOT_PARAMETER parameter{};
		parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		parameter.DescriptorTable.NumDescriptorRanges = 1;
		parameter.DescriptorTable.pDescriptorRanges = &range;
		parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_STATIC_SAMPLER_DESC sampler{};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_ROOT_SIGNATURE_DESC signatureDescription{};
		signatureDescription.NumParameters = 1;
		signatureDescription.pParameters = &parameter;
		signatureDescription.NumStaticSamplers = 1;
		signatureDescription.pStaticSamplers = &sampler;
		signatureDescription.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		Microsoft::WRL::ComPtr<ID3DBlob> serialized{};
		if (auto result = CheckHResult(D3D12SerializeRootSignature(&signatureDescription, D3D_ROOT_SIGNATURE_VERSION_1,
			serialized.GetAddressOf(), errors.ReleaseAndGetAddressOf()), "화면 출력 루트 시그니처 직렬화"); !result) return result;
		auto signature = CreateRootSignature(std::span(static_cast<const std::byte*>(serialized->GetBufferPointer()), serialized->GetBufferSize()));
		if (!signature) return std::unexpected(signature.error());
		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.pRootSignature = signature->Get();
		description.VS = { vertex->GetBufferPointer(), vertex->GetBufferSize() };
		description.PS = { pixel->GetBufferPointer(), pixel->GetBufferSize() };
		auto& blend = description.BlendState.RenderTarget[0];
		blend.SrcBlend = blend.SrcBlendAlpha = D3D12_BLEND_ONE;
		blend.DestBlend = blend.DestBlendAlpha = D3D12_BLEND_ZERO;
		blend.BlendOp = blend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blend.LogicOp = D3D12_LOGIC_OP_NOOP;
		blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		description.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		description.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		description.RasterizerState.DepthClipEnable = TRUE;
		description.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		description.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		description.DepthStencilState.FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
		description.DepthStencilState.BackFace = description.DepthStencilState.FrontFace;
		description.SampleMask = (std::numeric_limits<UINT>::max)();
		description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.SampleDesc.Count = 1;
		auto shader = CreateShader(description);
		if (!shader) return std::unexpected(shader.error());
		screenShader = std::move(*shader);
		return {};
	}
}
