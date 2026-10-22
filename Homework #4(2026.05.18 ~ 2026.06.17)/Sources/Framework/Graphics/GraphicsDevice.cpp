#include "Precompiled.hpp"
#include "GraphicsDevice.hpp"

#include "../Core/Memory.hpp"

namespace TUK::Framework
{
	GraphicsDevice::GraphicsDevice(
		ID3D12Device10* d3dDevice, 
		ID3D12CommandQueue* d3dCommandQueue,
		ID3D12RootSignature* d3dRootSignature) noexcept
		: d3dDevice(d3dDevice)
		, d3dCommandQueue(d3dCommandQueue)
		, d3dRootSignature(d3dRootSignature)
		, commandBuffers(Memory::GetPersistentResource())
		, currentCommandBufferIndex(0)
		, fences(Memory::GetPersistentResource())
		, currentFenceIndex(0)
	{
		assert(d3dDevice && d3dCommandQueue && d3dRootSignature);
	}

	GraphicsDevice::~GraphicsDevice() noexcept
	{
		commandBuffers.clear();
		fences.clear();

		if (d3dRootSignature)
		{
			d3dRootSignature->Release();
			d3dRootSignature = nullptr;
		}

		if (d3dCommandQueue)
		{
			d3dCommandQueue->Release();
			d3dCommandQueue = nullptr;
		}

		if (d3dDevice)
		{
			d3dDevice->Release();
			d3dDevice = nullptr;
		}
	}

	GraphicsDevice::GraphicsDevice(GraphicsDevice&& other) noexcept
		: d3dDevice(std::exchange(other.d3dDevice, nullptr))
		, d3dCommandQueue(std::exchange(other.d3dCommandQueue, nullptr))
		, d3dRootSignature(std::exchange(other.d3dRootSignature, nullptr))
		, commandBuffers(std::move(other.commandBuffers))
		, currentCommandBufferIndex(std::exchange(other.currentCommandBufferIndex, 0))
		, fences(std::move(other.fences))
		, currentFenceIndex(std::exchange(other.currentFenceIndex, 0))
	{
	}

	GraphicsDevice& GraphicsDevice::operator=(GraphicsDevice&& other) noexcept
	{
		if (this != &other)
		{
			commandBuffers.clear();
			fences.clear();

			if (d3dRootSignature)
			{
				d3dRootSignature->Release();
			}

			if (d3dCommandQueue)
			{
				d3dCommandQueue->Release();
			}

			if (d3dDevice)
			{
				d3dDevice->Release();
			}

			d3dDevice = std::exchange(other.d3dDevice, nullptr);
			d3dCommandQueue = std::exchange(other.d3dCommandQueue, nullptr);
			d3dRootSignature = std::exchange(other.d3dRootSignature, nullptr);
			commandBuffers = std::move(other.commandBuffers);
			currentCommandBufferIndex = std::exchange(other.currentCommandBufferIndex, 0);
			fences = std::move(other.fences);
			currentFenceIndex = std::exchange(other.currentFenceIndex, 0);
		}
		return *this;
	}

	std::expected<CommandBuffer, std::string> GraphicsDevice::CreateCommandBuffer()
	{
		assert(d3dDevice);

		ID3D12CommandAllocator* d3dCommandAllocator = nullptr;
		if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3dCommandAllocator))))
		{
			return std::unexpected("Failed to create command allocator");
		}

		ID3D12GraphicsCommandList4* d3dCommandList = nullptr;
		if (FAILED(d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			d3dCommandAllocator,
			nullptr,
			IID_PPV_ARGS(&d3dCommandList))))
		{
			d3dCommandAllocator->Release();
			return std::unexpected("Failed to create command list");
		}

		if (FAILED(d3dCommandList->Close()))
		{
			d3dCommandList->Release();
			d3dCommandAllocator->Release();
			return std::unexpected("Failed to close command list");
		}

		return CommandBuffer(d3dCommandAllocator, d3dCommandList);
	}

	std::expected<Fence, std::string> GraphicsDevice::CreateFence()
	{
		assert(d3dDevice);

		ID3D12Fence* d3dFence = nullptr;
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3dFence))))
		{
			return std::unexpected("Failed to create fence");
		}

		const HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent)
		{
			d3dFence->Release();
			return std::unexpected("Failed to create fence event");
		}

		return Fence(d3dFence, fenceEvent);
	}

	std::expected<Mesh, std::string> GraphicsDevice::CreateMesh(
		std::span<const Vertex> vertices, 
		std::span<const uint32_t> indices)
	{
		// 정점 버퍼 만들기
		ID3D12Resource2* vertexBuffer = nullptr;
		{
			const UINT bufferSize{ static_cast<UINT>(sizeof(Vertex) * vertices.size()) };

			D3D12_HEAP_PROPERTIES heapProps{};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

			D3D12_RESOURCE_DESC bufferDesc{};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Width = bufferSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			if (FAILED(d3dDevice->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer))))
			{
				return std::unexpected("Failed to create vertex buffer");
			}

			void* mappedData{ nullptr };
			if (FAILED(vertexBuffer->Map(0, nullptr, &mappedData)))
			{
				return std::unexpected("Failed to map vertex buffer");
			}
			std::memcpy(mappedData, vertices.data(), bufferSize);
			vertexBuffer->Unmap(0, nullptr);
		}

		// 인덱스 버퍼 만들기
		ID3D12Resource2* indexBuffer = nullptr;
		{
			const UINT bufferSize{ static_cast<UINT>(sizeof(uint32_t) * indices.size()) };

			D3D12_HEAP_PROPERTIES heapProps{};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

			D3D12_RESOURCE_DESC bufferDesc{};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Width = bufferSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			if (FAILED(d3dDevice->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer))))
			{
				return std::unexpected("Failed to create index buffer");
			}

			void* mappedData{ nullptr };
			if (FAILED(indexBuffer->Map(0, nullptr, &mappedData)))
			{
				return std::unexpected("Failed to map index buffer");
			}
			std::memcpy(mappedData, indices.data(), bufferSize);
			indexBuffer->Unmap(0, nullptr);
		}

		return Mesh(vertexBuffer, indexBuffer);
	}

	std::expected<Shader, std::string> GraphicsDevice::CreateShader(const std::filesystem::path& path, std::string_view entry)
	{
		UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#ifdef _DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		
		if (FAILED(D3DCompileFromFile(
			path.c_str(),
			nullptr,
			nullptr,
			entry.data(),
			"vs_5_0", 
			compileFlags, 0, &shaderBlob, &errorBlob)))
		{
			if (errorBlob)
			{
				const char* errorChars{ static_cast<const char*>(errorBlob->GetBufferPointer()) };
				const std::string errorText(errorChars, errorBlob->GetBufferSize());
				return std::unexpected<std::string>({ errorText.begin(), errorText.end() });
			}

			return std::unexpected<std::string>("Failed to compile Vertex Shader: " + path.string());
		}

		return Shader(shaderBlob);
	}

	std::expected<GraphicsPipeline, std::string> GraphicsDevice::CreateGraphicsPipeline(const Shader& vs, const Shader& ps)
	{
		D3D12_INPUT_ELEMENT_DESC instancedIl[]{
		   { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		   { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		   { "INSTANCE_WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		   { "INSTANCE_WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		   { "INSTANCE_WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		   { "INSTANCE_WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
		};

		D3D12_INPUT_ELEMENT_DESC uiIl[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pd{};
		pd.InputLayout = { instancedIl, 6 };
		pd.pRootSignature = d3dRootSignature;
		pd.VS = { vs.GetBlob()->GetBufferPointer(), vs.GetBlob()->GetBufferSize() };
		pd.PS = { ps.GetBlob()->GetBufferPointer(), ps.GetBlob()->GetBufferSize() };
		pd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		pd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pd.DepthStencilState.DepthEnable = TRUE;
		pd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pd.SampleMask = UINT_MAX;
		pd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pd.NumRenderTargets = 2;
		pd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pd.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pd.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		pd.SampleDesc.Count = 1;

		ID3D12PipelineState* pipelineState = nullptr;
		if (FAILED(d3dDevice->CreateGraphicsPipelineState(&pd, IID_PPV_ARGS(&pipelineState))))
		{
			return std::unexpected<std::string>("Failed to create gameObjectPipelineState.");
		}

		return GraphicsPipeline(pipelineState);
	}

	std::expected<std::reference_wrapper<CommandBuffer>, std::string> GraphicsDevice::AcquireCommandBuffer()
	{
		constexpr std::size_t defaultBufferCount = 2;
		while (commandBuffers.size() < defaultBufferCount)
		{
			auto expectedCommandBuffer = CreateCommandBuffer();
			if (!expectedCommandBuffer.has_value())
			{
				return std::unexpected(expectedCommandBuffer.error());
			}
			commandBuffers.push_back(std::move(*expectedCommandBuffer));
		}

		const std::size_t index = currentCommandBufferIndex;
		CommandBuffer& commandBuffer = commandBuffers[index];
		if (FAILED(commandBuffer.d3dCommandAllocator->Reset()))
		{
			return std::unexpected("Failed to reset command allocator");
		}

		if (FAILED(commandBuffer.d3dCommandList->Reset(commandBuffer.d3dCommandAllocator, nullptr)))
		{
			return std::unexpected("Failed to reset command list");
		}

		commandBuffer.currentRenderTargets.clear();
		commandBuffer.d3dCommandList->SetGraphicsRootSignature(d3dRootSignature);
		currentCommandBufferIndex = (currentCommandBufferIndex + 1) % commandBuffers.size();
		return std::ref(commandBuffer);
	}

	std::expected<void, std::string> GraphicsDevice::SubmitCommandBuffer(CommandBuffer& commandBuffer)
	{
		assert(d3dCommandQueue);

		if (!commandBuffer.currentRenderTargets.empty())
		{
			return std::unexpected("End the render pass before submitting the command buffer");
		}

		if (FAILED(commandBuffer.d3dCommandList->Close()))
		{
			return std::unexpected("Failed to close command list");
		}

		ID3D12CommandList* commandLists[]{ &commandBuffer.GetD3D12CommandList() };
		d3dCommandQueue->ExecuteCommandLists(1, commandLists);
		return {};
	}

	std::expected<std::reference_wrapper<Fence>, std::string> GraphicsDevice::AcquireFence()
	{
		if (fences.empty())
		{
			std::expected<Fence, std::string> expectedFence = CreateFence();
			if (!expectedFence.has_value())
			{
				return std::unexpected(expectedFence.error());
			}
			fences.push_back(std::move(*expectedFence));
		}

		const std::size_t index = currentFenceIndex;
		currentFenceIndex = (currentFenceIndex + 1) % fences.size();
		return std::ref(fences[index]);
	}

	ID3D12Device10& GraphicsDevice::GetD3D12Device() const noexcept
	{
		assert(d3dDevice);
		return *d3dDevice;
	}

	ID3D12CommandQueue* GraphicsDevice::GetD3D12CommandQueue() const noexcept
	{
		return d3dCommandQueue;
	}
}
