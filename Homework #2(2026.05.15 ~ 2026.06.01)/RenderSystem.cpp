#include "Precompiled.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "RendererOptions.h"

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
}

Microsoft::WRL::ComPtr<IDXGIFactory4> RenderSystem::factory;
Microsoft::WRL::ComPtr<ID3D12Device> RenderSystem::device;
Microsoft::WRL::ComPtr<ID3D12CommandQueue> RenderSystem::commandQueue;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> RenderSystem::commandList;
Microsoft::WRL::ComPtr<ID3D12RootSignature> RenderSystem::meshRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> RenderSystem::meshPipelineState;
bool RenderSystem::tearingSupported = false;

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
