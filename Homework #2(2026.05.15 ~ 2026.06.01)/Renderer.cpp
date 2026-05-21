#include "Precompiled.h"
#include "Renderer.h"

#include "Material.h"
#include "RenderTarget.h"
#include "RenderTargetOptions.h"
#include "Shader.h"

namespace
{
	[[nodiscard]] D3D12_RASTERIZER_DESC CreateDefaultRasterizerDesc() noexcept
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

	[[nodiscard]] D3D12_BLEND_DESC CreateDefaultBlendDesc() noexcept
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

	[[nodiscard]] D3D12_BLEND_DESC CreateAlphaBlendDesc() noexcept
	{
		D3D12_BLEND_DESC desc{ CreateDefaultBlendDesc() };
		D3D12_RENDER_TARGET_BLEND_DESC& target{ desc.RenderTarget[0] };
		target.BlendEnable = TRUE;
		target.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		target.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		target.BlendOp = D3D12_BLEND_OP_ADD;
		target.SrcBlendAlpha = D3D12_BLEND_ONE;
		target.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		target.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		return desc;
	}

	[[nodiscard]] std::size_t HashShaderPair(const Shader* vertexShader_, const Shader* pixelShader_) noexcept
	{
		const std::size_t vertexHash{ std::hash<const Shader*>{}(vertexShader_) };
		const std::size_t pixelHash{ std::hash<const Shader*>{}(pixelShader_) };
		return vertexHash ^ (pixelHash + 0x9e3779b97f4a7c15ULL + (vertexHash << 6) + (vertexHash >> 2));
	}
}

Microsoft::WRL::ComPtr<IDXGIFactory4> Renderer::factory;
Microsoft::WRL::ComPtr<ID3D12Device> Renderer::device;
Microsoft::WRL::ComPtr<ID3D12CommandQueue> Renderer::commandQueue;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Renderer::commandList;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Renderer::meshRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Renderer::meshPipelineState;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Renderer::uiRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Renderer::uiPipelineState;
std::unordered_map<std::size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> Renderer::meshMaterialPipelineStates;
std::unordered_map<std::size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> Renderer::uiMaterialPipelineStates;
bool Renderer::tearingSupported = false;

bool Renderer::Initialize()
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

	if (!CreateUIPipeline())
	{
		return false;
	}

	return true;
}

void Renderer::Shutdown() noexcept
{
	uiMaterialPipelineStates.clear();
	meshMaterialPipelineStates.clear();
	uiPipelineState.Reset();
	uiRootSignature.Reset();
	meshPipelineState.Reset();
	meshRootSignature.Reset();
	commandList.Reset();
	commandQueue.Reset();
	device.Reset();
	factory.Reset();
	tearingSupported = false;
}

std::unique_ptr<RenderTarget> Renderer::CreateRenderTarget(const RenderTargetOptions& options_)
{
	std::unique_ptr<RenderTarget> renderTarget{ new RenderTarget() };
	if (!renderTarget->Initialize(options_))
	{
		return nullptr;
	}

	return renderTarget;
}

bool Renderer::CheckTearingSupport()
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

bool Renderer::CreateMeshPipeline()
{
	D3D12_ROOT_PARAMETER rootParameters[3]{};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[0].Descriptor.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.ShaderRegister = 1;
	rootParameters[1].Descriptor.RegisterSpace = 0;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].Descriptor.ShaderRegister = 2;
	rootParameters[2].Descriptor.RegisterSpace = 0;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.NumParameters = static_cast<UINT>(sizeof(rootParameters) / sizeof(rootParameters[0]));
	rootSignatureDesc.pParameters = rootParameters;
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

	Shader vertexShader;
	if (!vertexShader.Compile(L"Assets/GameObject.hlsl", "VSMain", "vs_5_0"))
	{
		return false;
	}

	Shader pixelShader;
	if (!pixelShader.Compile(L"Assets/GameObject.hlsl", "PSMain", "ps_5_0"))
	{
		return false;
	}

	return CreateMeshPipelineState(
		vertexShader.GetShaderByteCode(),
		pixelShader.GetShaderByteCode(),
		meshPipelineState.ReleaseAndGetAddressOf());
}

bool Renderer::CreateUIPipeline()
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
		IID_PPV_ARGS(&uiRootSignature))))
	{
		return false;
	}

	Shader vertexShader;
	if (!vertexShader.Compile(L"Assets/UIObject.hlsl", "VSMain", "vs_5_0"))
	{
		return false;
	}

	Shader pixelShader;
	if (!pixelShader.Compile(L"Assets/UIObject.hlsl", "PSMain", "ps_5_0"))
	{
		return false;
	}

	return CreateUIPipelineState(
		vertexShader.GetShaderByteCode(),
		pixelShader.GetShaderByteCode(),
		uiPipelineState.ReleaseAndGetAddressOf());
}

ID3D12PipelineState* Renderer::GetMeshPipelineState(const Material& material_)
{
	const Shader* const vertexShader{ material_.GetVertexShader() };
	const Shader* const pixelShader{ material_.GetPixelShader() };
	if (nullptr == vertexShader || nullptr == pixelShader || !vertexShader->HasByteCode() || !pixelShader->HasByteCode())
	{
		return meshPipelineState.Get();
	}

	const std::size_t key{ HashShaderPair(vertexShader, pixelShader) };
	const auto iterator{ meshMaterialPipelineStates.find(key) };
	if (iterator != meshMaterialPipelineStates.end())
	{
		return iterator->second.Get();
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	if (!CreateMeshPipelineState(
		vertexShader->GetShaderByteCode(),
		pixelShader->GetShaderByteCode(),
		pipelineState.GetAddressOf()))
	{
		return meshPipelineState.Get();
	}

	ID3D12PipelineState* const createdPipelineState{ pipelineState.Get() };
	meshMaterialPipelineStates.emplace(key, std::move(pipelineState));
	return createdPipelineState;
}

ID3D12PipelineState* Renderer::GetUIPipelineState(const Material& material_)
{
	const Shader* const vertexShader{ material_.GetVertexShader() };
	const Shader* const pixelShader{ material_.GetPixelShader() };
	if (nullptr == vertexShader || nullptr == pixelShader || !vertexShader->HasByteCode() || !pixelShader->HasByteCode())
	{
		return uiPipelineState.Get();
	}

	const std::size_t key{ HashShaderPair(vertexShader, pixelShader) };
	const auto iterator{ uiMaterialPipelineStates.find(key) };
	if (iterator != uiMaterialPipelineStates.end())
	{
		return iterator->second.Get();
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	if (!CreateUIPipelineState(
		vertexShader->GetShaderByteCode(),
		pixelShader->GetShaderByteCode(),
		pipelineState.GetAddressOf()))
	{
		return uiPipelineState.Get();
	}

	ID3D12PipelineState* const createdPipelineState{ pipelineState.Get() };
	uiMaterialPipelineStates.emplace(key, std::move(pipelineState));
	return createdPipelineState;
}

bool Renderer::CreateMeshPipelineState(
	const D3D12_SHADER_BYTECODE& vertexShader_,
	const D3D12_SHADER_BYTECODE& pixelShader_,
	ID3D12PipelineState** pipelineState_)
{
	if (nullptr == pipelineState_ || nullptr == meshRootSignature)
	{
		return false;
	}

	const D3D12_INPUT_ELEMENT_DESC inputElements[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INSTANCEWORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEWORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEWORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEWORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	pipelineDesc.pRootSignature = meshRootSignature.Get();
	pipelineDesc.VS = vertexShader_;
	pipelineDesc.PS = pixelShader_;
	pipelineDesc.BlendState = CreateDefaultBlendDesc();
	pipelineDesc.SampleMask = UINT_MAX;
	pipelineDesc.RasterizerState = CreateDefaultRasterizerDesc();
	pipelineDesc.DepthStencilState.DepthEnable = TRUE;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	pipelineDesc.DepthStencilState.StencilEnable = FALSE;
	pipelineDesc.InputLayout = { inputElements, static_cast<UINT>(sizeof(inputElements) / sizeof(inputElements[0])) };
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineDesc.SampleDesc.Count = 1;
	pipelineDesc.SampleDesc.Quality = 0;

	return SUCCEEDED(device->CreateGraphicsPipelineState(
		&pipelineDesc,
		IID_PPV_ARGS(pipelineState_)));
}

bool Renderer::CreateUIPipelineState(
	const D3D12_SHADER_BYTECODE& vertexShader_,
	const D3D12_SHADER_BYTECODE& pixelShader_,
	ID3D12PipelineState** pipelineState_)
{
	if (nullptr == pipelineState_ || nullptr == uiRootSignature)
	{
		return false;
	}

	const D3D12_INPUT_ELEMENT_DESC inputElements[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	pipelineDesc.pRootSignature = uiRootSignature.Get();
	pipelineDesc.VS = vertexShader_;
	pipelineDesc.PS = pixelShader_;
	pipelineDesc.BlendState = CreateAlphaBlendDesc();
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
		IID_PPV_ARGS(pipelineState_)));
}
