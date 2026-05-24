#include "Precompiled.h"
#include "Shader.h"

#include "Renderer.h"
#include "RootParameterSlot.h"

bool Shader::LoadFromFile(ID3D12Device* device_, const std::filesystem::path& path_)
{
	if (nullptr == device_)
	{
		return false;
	}

	std::ofstream clearLog{ "Resources/LastShaderError.log", std::ios::trunc };

	UINT compileFlags{ 0 };
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	if (FAILED(D3DCompileFromFile(
		path_.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSMain",
		"vs_5_0",
		compileFlags,
		0,
		&vertexShader,
		&errorBlob)))
	{
		return false;
	}

	errorBlob.Reset();

	if (FAILED(D3DCompileFromFile(
		path_.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSMain",
		"ps_5_0",
		compileFlags,
		0,
		&pixelShader,
		&errorBlob)))
	{
		return false;
	}

	D3D12_ROOT_PARAMETER rootParameters[3]{};
	rootParameters[(UINT)RootParameterSlot::Camera].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[(UINT)RootParameterSlot::Camera].Descriptor.ShaderRegister = 0;
	rootParameters[(UINT)RootParameterSlot::Camera].Descriptor.RegisterSpace = 0;
	rootParameters[(UINT)RootParameterSlot::Camera].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameters[(UINT)RootParameterSlot::Object].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[(UINT)RootParameterSlot::Object].Descriptor.ShaderRegister = 1;
	rootParameters[(UINT)RootParameterSlot::Object].Descriptor.RegisterSpace = 0;
	rootParameters[(UINT)RootParameterSlot::Object].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameters[(UINT)RootParameterSlot::Material].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[(UINT)RootParameterSlot::Material].Descriptor.ShaderRegister = 2;
	rootParameters[(UINT)RootParameterSlot::Material].Descriptor.RegisterSpace = 0;
	rootParameters[(UINT)RootParameterSlot::Material].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDescription{};
	rootSignatureDescription.NumParameters = static_cast<UINT>(std::size(rootParameters));
	rootSignatureDescription.pParameters = rootParameters;
	rootSignatureDescription.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	errorBlob.Reset();

	if (FAILED(D3D12SerializeRootSignature(
		&rootSignatureDescription,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob)))
	{
		return false;
	}

	if (FAILED(device_->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&graphicsRootSignature))))
	{
		return false;
	}

	D3D12_INPUT_ELEMENT_DESC inputElements[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INSTANCEWORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEWORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEWORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEWORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDescription{};
	pipelineDescription.pRootSignature = graphicsRootSignature.Get();
	pipelineDescription.VS.pShaderBytecode = vertexShader->GetBufferPointer();
	pipelineDescription.VS.BytecodeLength = vertexShader->GetBufferSize();
	pipelineDescription.PS.pShaderBytecode = pixelShader->GetBufferPointer();
	pipelineDescription.PS.BytecodeLength = pixelShader->GetBufferSize();
	pipelineDescription.InputLayout.pInputElementDescs = inputElements;
	pipelineDescription.InputLayout.NumElements = static_cast<UINT>(std::size(inputElements));
	pipelineDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDescription.NumRenderTargets = 1;
	pipelineDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineDescription.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineDescription.SampleDesc.Count = 1;
	pipelineDescription.SampleMask = UINT_MAX;

	pipelineDescription.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDescription.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineDescription.RasterizerState.FrontCounterClockwise = FALSE;
	pipelineDescription.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	pipelineDescription.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	pipelineDescription.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	pipelineDescription.RasterizerState.DepthClipEnable = TRUE;

	pipelineDescription.BlendState.RenderTarget[0].BlendEnable = FALSE;
	pipelineDescription.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
	pipelineDescription.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	pipelineDescription.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	pipelineDescription.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	pipelineDescription.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	pipelineDescription.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	pipelineDescription.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	pipelineDescription.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	pipelineDescription.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	pipelineDescription.DepthStencilState.DepthEnable = TRUE;
	pipelineDescription.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDescription.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDescription.DepthStencilState.StencilEnable = FALSE;

	if (FAILED(device_->CreateGraphicsPipelineState(&pipelineDescription, IID_PPV_ARGS(&pipelineState))))
	{
		return false;
	}

	SetPath(path_);
	SetLoaded(true);
	return true;
}

uint64_t Shader::GetPipelineId() const noexcept
{
	return pipelineId;
}

void Shader::SetPipelineId(uint64_t pipelineId_) noexcept
{
	pipelineId = pipelineId_;
}

ID3D12PipelineState* Shader::GetPipelineState() noexcept
{
	return pipelineState.Get();
}

const ID3D12PipelineState* Shader::GetPipelineState() const noexcept
{
	return pipelineState.Get();
}

void Shader::SetPipelineState(ID3D12PipelineState* pipelineState_) noexcept
{
	pipelineState = pipelineState_;
}

ID3D12RootSignature* Shader::GetGraphicsRootSignature() noexcept
{
	return graphicsRootSignature.Get();
}

const ID3D12RootSignature* Shader::GetGraphicsRootSignature() const noexcept
{
	return graphicsRootSignature.Get();
}

void Shader::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature_) noexcept
{
	graphicsRootSignature = rootSignature_;
}
