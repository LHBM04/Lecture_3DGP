#include "Precompiled.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "RendererOptions.h"
#include "Mesh.h"

namespace
{
	[[nodiscard]] D3D12_RESOURCE_DESC CreateBufferDesc(const UINT64 size_)
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = size_;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		return desc;
	}

	[[nodiscard]] D3D12_HEAP_PROPERTIES CreateUploadHeapProperties()
	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;
		return heapProperties;
	}

	[[nodiscard]] D3D12_RASTERIZER_DESC CreateDefaultRasterizerDesc()
	{
		D3D12_RASTERIZER_DESC desc{};
		desc.FillMode = D3D12_FILL_MODE_SOLID;
		desc.CullMode = D3D12_CULL_MODE_NONE;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		desc.DepthClipEnable = TRUE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		desc.ForcedSampleCount = 0;
		desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		return desc;
	}

	[[nodiscard]] D3D12_BLEND_DESC CreateDefaultBlendDesc()
	{
		D3D12_BLEND_DESC desc{};
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;

		D3D12_RENDER_TARGET_BLEND_DESC renderTargetDesc{};
		renderTargetDesc.BlendEnable = FALSE;
		renderTargetDesc.LogicOpEnable = FALSE;
		renderTargetDesc.SrcBlend = D3D12_BLEND_ONE;
		renderTargetDesc.DestBlend = D3D12_BLEND_ZERO;
		renderTargetDesc.BlendOp = D3D12_BLEND_OP_ADD;
		renderTargetDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		renderTargetDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		renderTargetDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		renderTargetDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		renderTargetDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		for (D3D12_RENDER_TARGET_BLEND_DESC& target : desc.RenderTarget)
		{
			target = renderTargetDesc;
		}

		return desc;
	}
}

Microsoft::WRL::ComPtr<IDXGIFactory4> RenderSystem::factory;
Microsoft::WRL::ComPtr<ID3D12Device> RenderSystem::device;
Microsoft::WRL::ComPtr<ID3D12CommandQueue> RenderSystem::commandQueue;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> RenderSystem::commandList;
Microsoft::WRL::ComPtr<ID3D12RootSignature> RenderSystem::meshRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> RenderSystem::meshPipelineState;
bool RenderSystem::tearingSupported = false;

Renderer::~Renderer() noexcept
{
	WaitForGPU();

	if (nullptr != fenceEvent)
	{
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}
}

bool Renderer::Initialize(const RendererOptions& options_)
{
	options = options_;
	isVSyncEnabled = options.vSync;
	isTearingEnabled = options.allowTearing && RenderSystem::tearingSupported;
	clearColor = options.clearColor;
	UpdateViewportAndScissor();

	if (nullptr == options.window || options.width <= 0 || options.height <= 0)
	{
		return false;
	}

	if (!CreateSwapChain(options))
	{
		return false;
	}

	if (!CreateRenderTargetViews())
	{
		return false;
	}

	if (!CreateFrameResources())
	{
		return false;
	}

	if (!CreateFence())
	{
		return false;
	}

	return true;
}

bool Renderer::CreateSwapChain(const RendererOptions& options_)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = static_cast<UINT>(options_.width);
	swapChainDesc.Height = static_cast<UINT>(options_.height);
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = isTearingEnabled ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> createdSwapChain;
	const HRESULT result = RenderSystem::factory->CreateSwapChainForHwnd(
		RenderSystem::commandQueue.Get(),
		options_.window,
		&swapChainDesc,
		nullptr,
		nullptr,
		&createdSwapChain);

	if (FAILED(result))
	{
		return false;
	}

	if (FAILED(createdSwapChain.As(&swapChain)))
	{
		return false;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	return true;
}

bool Renderer::CreateRenderTargetViews()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = bufferCount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	if (FAILED(RenderSystem::device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&renderTargetViewHeap))))
	{
		return false;
	}

	renderTargetViewDescriptorSize =
		RenderSystem::device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	backBuffers.resize(bufferCount);

	D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle =
		renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < bufferCount; ++i)
	{
		if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]))))
		{
			return false;
		}

		RenderSystem::device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, descriptorHandle);
		descriptorHandle.ptr += renderTargetViewDescriptorSize;
	}

	return true;
}

bool Renderer::CreateFrameResources()
{
	commandAllocators.resize(bufferCount);

	for (UINT i = 0; i < bufferCount; ++i)
	{
		if (FAILED(RenderSystem::device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocators[i]))))
		{
			return false;
		}
	}

	return true;
}

bool Renderer::CreateFence()
{
	if (FAILED(RenderSystem::device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence))))
	{
		return false;
	}

	fenceValue = 1;
	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	return nullptr != fenceEvent;
}

void Renderer::Resize(int width_, int height_)
{
	if (width_ <= 0 || height_ <= 0 || nullptr == swapChain)
	{
		return;
	}

	WaitForGPU();
	ReleaseBackBuffers();

	options.width = width_;
	options.height = height_;
	UpdateViewportAndScissor();

	const UINT flags = isTearingEnabled ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	const HRESULT result = swapChain->ResizeBuffers(
		bufferCount,
		static_cast<UINT>(width_),
		static_cast<UINT>(height_),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		flags);

	if (FAILED(result))
	{
		return;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	if (!CreateRenderTargetViews())
	{
		ReleaseBackBuffers();
	}
}

bool Renderer::ShouldVSync() const noexcept
{
	return isVSyncEnabled;
}

void Renderer::SetShouldVSync(bool enabled_) noexcept
{
	isVSyncEnabled = enabled_;
}

bool Renderer::ShouldTearing() const noexcept
{
	return isTearingEnabled;
}

void Renderer::SetShouldTearing(bool enabled_) noexcept
{
	isTearingEnabled = enabled_ && RenderSystem::tearingSupported;
}

void Renderer::PreRender()
{
	if (isRecording)
	{
		return;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	ID3D12CommandAllocator* const commandAllocator = commandAllocators[frameIndex].Get();
	commandAllocator->Reset();
	RenderSystem::commandList->Reset(commandAllocator, nullptr);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	RenderSystem::commandList->ResourceBarrier(1, &barrier);
	isRecording = true;
}

void Renderer::PostRender()
{
	if (!isRecording)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	RenderSystem::commandList->ResourceBarrier(1, &barrier);
	RenderSystem::commandList->Close();

	ID3D12CommandList* commandLists[] = { RenderSystem::commandList.Get() };
	RenderSystem::commandQueue->ExecuteCommandLists(1, commandLists);

	isRecording = false;
}

void Renderer::Clear()
{
	if (!isRecording)
	{
		return;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle =
		renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
	descriptorHandle.ptr += static_cast<SIZE_T>(frameIndex) * renderTargetViewDescriptorSize;

	const float clearColorValues[4]{
		std::clamp(clearColor.x, 0.0f, 1.0f),
		std::clamp(clearColor.y, 0.0f, 1.0f),
		std::clamp(clearColor.z, 0.0f, 1.0f),
		1.0f
	};

	RenderSystem::commandList->OMSetRenderTargets(1, &descriptorHandle, FALSE, nullptr);
	RenderSystem::commandList->ClearRenderTargetView(descriptorHandle, clearColorValues, 0, nullptr);
}

void Renderer::DrawMesh(const Mesh& mesh_)
{
	if (!isRecording || nullptr == RenderSystem::meshPipelineState || nullptr == RenderSystem::meshRootSignature)
	{
		return;
	}

	const std::vector<MeshVertex>& vertices = mesh_.GetVertices();
	const std::vector<std::uint32_t>& indices = mesh_.GetIndices();
	if (vertices.empty() || indices.empty())
	{
		return;
	}

	const UINT64 vertexBufferSize = static_cast<UINT64>(sizeof(MeshVertex) * vertices.size());
	const UINT64 indexBufferSize = static_cast<UINT64>(sizeof(std::uint32_t) * indices.size());
	const D3D12_HEAP_PROPERTIES heapProperties = CreateUploadHeapProperties();
	const D3D12_RESOURCE_DESC vertexBufferDesc = CreateBufferDesc(vertexBufferSize);
	const D3D12_RESOURCE_DESC indexBufferDesc = CreateBufferDesc(indexBufferSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	if (FAILED(RenderSystem::device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer))))
	{
		return;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	if (FAILED(RenderSystem::device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer))))
	{
		return;
	}

	void* mappedData{ nullptr };
	if (FAILED(vertexBuffer->Map(0, nullptr, &mappedData)))
	{
		return;
	}

	std::memcpy(mappedData, vertices.data(), static_cast<size_t>(vertexBufferSize));
	vertexBuffer->Unmap(0, nullptr);

	if (FAILED(indexBuffer->Map(0, nullptr, &mappedData)))
	{
		return;
	}

	std::memcpy(mappedData, indices.data(), static_cast<size_t>(indexBufferSize));
	indexBuffer->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
	vertexBufferView.StrideInBytes = sizeof(MeshVertex);

	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	RenderSystem::commandList->SetGraphicsRootSignature(RenderSystem::meshRootSignature.Get());
	RenderSystem::commandList->SetPipelineState(RenderSystem::meshPipelineState.Get());
	RenderSystem::commandList->RSSetViewports(1, &viewport);
	RenderSystem::commandList->RSSetScissorRects(1, &scissorRect);
	RenderSystem::commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderSystem::commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	RenderSystem::commandList->IASetIndexBuffer(&indexBufferView);
	RenderSystem::commandList->DrawIndexedInstanced(static_cast<UINT>(indices.size()), 1, 0, 0, 0);

	uploadResources.emplace_back(std::move(vertexBuffer));
	uploadResources.emplace_back(std::move(indexBuffer));
}

void Renderer::Present()
{
	PostRender();

	const UINT syncInterval = isVSyncEnabled ? 1 : 0;
	const UINT flags = (!isVSyncEnabled && isTearingEnabled) ? DXGI_PRESENT_ALLOW_TEARING : 0;
	swapChain->Present(syncInterval, flags);

	WaitForGPU();
	uploadResources.clear();
	frameIndex = swapChain->GetCurrentBackBufferIndex();
}

void Renderer::UpdateViewportAndScissor() noexcept
{
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(std::max(options.width, 0));
	viewport.Height = static_cast<float>(std::max(options.height, 0));
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = std::max(options.width, 0);
	scissorRect.bottom = std::max(options.height, 0);
}

void Renderer::WaitForGPU() noexcept
{
	if (nullptr == fenceEvent || nullptr == fence || nullptr == RenderSystem::commandQueue)
	{
		return;
	}

	const UINT64 currentFenceValue = fenceValue;
	if (FAILED(RenderSystem::commandQueue->Signal(fence.Get(), currentFenceValue)))
	{
		return;
	}

	++fenceValue;

	if (fence->GetCompletedValue() < currentFenceValue)
	{
		if (SUCCEEDED(fence->SetEventOnCompletion(currentFenceValue, fenceEvent)))
		{
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}
}

void Renderer::ReleaseBackBuffers() noexcept
{
	for (Microsoft::WRL::ComPtr<ID3D12Resource>& backBuffer : backBuffers)
	{
		backBuffer.Reset();
	}

	backBuffers.clear();
	renderTargetViewHeap.Reset();
}

bool RenderSystem::Initialize()
{
	UINT factoryFlags = 0;

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		return false;
	}

	tearingSupported = CheckTearingSupport();

	if (FAILED(D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device))))
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> temporaryAllocator;
	if (FAILED(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&temporaryAllocator))))
	{
		return false;
	}

	if (FAILED(device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		temporaryAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList))))
	{
		return false;
	}

	commandList->Close();
	if (!CreateMeshPipeline())
	{
		return false;
	}

	return true;
}

void RenderSystem::Shutdown() noexcept
{
	meshPipelineState.Reset();
	meshRootSignature.Reset();
	commandList.Reset();
	commandQueue.Reset();
	device.Reset();
	factory.Reset();
	tearingSupported = false;
}

std::unique_ptr<Renderer> RenderSystem::CreateRenderer(const RendererOptions& options_)
{
	std::unique_ptr<Renderer> renderer{ new Renderer() };
	if (!renderer->Initialize(options_))
	{
		return nullptr;
	}

	return renderer;
}

bool RenderSystem::CheckTearingSupport()
{
	Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
	if (FAILED(factory.As(&factory5)))
	{
		return false;
	}

	BOOL allowTearing = FALSE;
	if (FAILED(factory5->CheckFeatureSupport(
		DXGI_FEATURE_PRESENT_ALLOW_TEARING,
		&allowTearing,
		sizeof(allowTearing))))
	{
		return false;
	}

	return TRUE == allowTearing;
}

bool RenderSystem::CreateMeshPipeline()
{
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.NumParameters = 0;
	rootSignatureDesc.pParameters = nullptr;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	if (FAILED(D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&rootSignatureBlob,
		&errorBlob)))
	{
		return false;
	}

	if (FAILED(device->CreateRootSignature(
		0,
		rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&meshRootSignature))))
	{
		return false;
	}

	constexpr char shaderSource[] = R"(
struct VSInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

PSInput VSMain(VSInput input)
{
	PSInput output;
	output.position = float4(input.position.xy, input.position.z * 0.5f + 0.5f, 1.0f);
	output.color = abs(input.normal);
	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}
)";

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(D3DCompile(
		shaderSource,
		sizeof(shaderSource),
		nullptr,
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		compileFlags,
		0,
		&vertexShader,
		&errorBlob)))
	{
		return false;
	}

	if (FAILED(D3DCompile(
		shaderSource,
		sizeof(shaderSource),
		nullptr,
		nullptr,
		nullptr,
		"PSMain",
		"ps_5_0",
		compileFlags,
		0,
		&pixelShader,
		&errorBlob)))
	{
		return false;
	}

	const D3D12_INPUT_ELEMENT_DESC inputElements[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	pipelineDesc.pRootSignature = meshRootSignature.Get();
	pipelineDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	pipelineDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
	pipelineDesc.BlendState = CreateDefaultBlendDesc();
	pipelineDesc.SampleMask = UINT_MAX;
	pipelineDesc.RasterizerState = CreateDefaultRasterizerDesc();
	pipelineDesc.DepthStencilState.DepthEnable = FALSE;
	pipelineDesc.DepthStencilState.StencilEnable = FALSE;
	pipelineDesc.InputLayout = { inputElements, static_cast<UINT>(sizeof(inputElements) / sizeof(inputElements[0])) };
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineDesc.SampleDesc.Count = 1;
	pipelineDesc.SampleDesc.Quality = 0;

	return SUCCEEDED(device->CreateGraphicsPipelineState(
		&pipelineDesc,
		IID_PPV_ARGS(&meshPipelineState)));
}
