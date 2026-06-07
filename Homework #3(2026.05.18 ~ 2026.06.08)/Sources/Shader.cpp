#include "Precompiled.h"

#include "Shader.h"

#include <d3dcompiler.h>

#include "Logger.h"
#include "Mesh.h"

Shader::~Shader()
{
	Unload();
}

bool Shader::Load()
{
	const std::expected<void, std::wstring> result{ Compile() };
	if (!result.has_value())
	{
		Logger::Critical(L"Compile failed: {}", result.error());
		return false;
	}
	return true;
}

void Shader::Unload()
{
	pipelineState.Reset();
	vsBlob.Reset();
	psBlob.Reset();
}

std::expected<void, std::wstring> Shader::Compile(std::wstring_view vsEntry_, std::wstring_view psEntry_)
{
	UINT compileFlags{ 0 };
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	std::string vsEntry(vsEntry_.size(), '\0');
	std::ranges::transform(vsEntry_, vsEntry.begin(), [](wchar_t character_)
	{
		return static_cast<char>(character_);
	});

	std::string psEntry(psEntry_.size(), '\0');
	std::ranges::transform(psEntry_, psEntry.begin(), [](wchar_t character_)
	{
		return static_cast<char>(character_);
	});
	
	// Vertex Shader
	HRESULT hr = D3DCompileFromFile(
		path.c_str(), nullptr, nullptr, 
		vsEntry.c_str(), 
		"vs_5_0", compileFlags, 0, &vsBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			const char* errorChars{ static_cast<const char*>(errorBlob->GetBufferPointer()) };
			const std::string errorText(errorChars, errorBlob->GetBufferSize());
			return std::unexpected<std::wstring>(std::wstring(errorText.begin(), errorText.end()));
		}
		return std::unexpected<std::wstring>(L"Failed to compile Vertex Shader: " + path);
	}

	// Pixel Shader
	hr = D3DCompileFromFile(
		path.c_str(), nullptr, nullptr, 
		psEntry.c_str(), 
		"ps_5_0", compileFlags, 0, &psBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			const char* errorChars{ static_cast<const char*>(errorBlob->GetBufferPointer()) };
			const std::string errorText(errorChars, errorBlob->GetBufferSize());
			return std::unexpected<std::wstring>(std::wstring(errorText.begin(), errorText.end()));
		}
		return std::unexpected<std::wstring>(L"Failed to compile Pixel Shader: " + path);
	}

	return {};
}

bool Shader::CreatePipelineState(
	ID3D12Device* device_,
	ID3D12RootSignature* rootSignature_,
	DXGI_FORMAT renderTargetFormat_,
	DXGI_FORMAT depthStencilFormat_)
{
	if (pipelineState != nullptr)
	{
		return true;
	}

	if (device_ == nullptr || rootSignature_ == nullptr)
	{
		return false;
	}

	if (vsBlob == nullptr || psBlob == nullptr)
	{
		const std::expected<void, std::wstring> result{ Compile() };
		if (!result.has_value())
		{
			Logger::Critical(L"Compile failed: {}", result.error());
			return false;
		}
	}

	const D3D12_INPUT_ELEMENT_DESC inputElements[]
	{
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = sizeof(Vector3D),
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "INSTANCE_WORLD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 1,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
			.InstanceDataStepRate = 1,
		},
		{
			.SemanticName = "INSTANCE_WORLD",
			.SemanticIndex = 1,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 1,
			.AlignedByteOffset = sizeof(float) * 4,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
			.InstanceDataStepRate = 1,
		},
		{
			.SemanticName = "INSTANCE_WORLD",
			.SemanticIndex = 2,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 1,
			.AlignedByteOffset = sizeof(float) * 8,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
			.InstanceDataStepRate = 1,
		},
		{
			.SemanticName = "INSTANCE_WORLD",
			.SemanticIndex = 3,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 1,
			.AlignedByteOffset = sizeof(float) * 12,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
			.InstanceDataStepRate = 1,
		},
	};

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	for (D3D12_RENDER_TARGET_BLEND_DESC& renderTargetBlendDesc : blendDesc.RenderTarget)
	{
		renderTargetBlendDesc.BlendEnable = FALSE;
		renderTargetBlendDesc.LogicOpEnable = FALSE;
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
		renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		renderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = depthStencilFormat_ != DXGI_FORMAT_UNKNOWN;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = rootSignature_;
	psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
	psoDesc.BlendState = blendDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = depthStencilDesc;
	psoDesc.InputLayout = { inputElements, static_cast<UINT>(std::size(inputElements)) };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = renderTargetFormat_;
	psoDesc.DSVFormat = depthStencilFormat_;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	if (FAILED(device_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState))))
	{
		Logger::Critical(L"Failed to create graphics pipeline state: {}", path);
		return false;
	}

	return true;
}

ID3DBlob* Shader::GetVSBlob() const noexcept
{
	return vsBlob.Get();
}

ID3DBlob* Shader::GetPSBlob() const noexcept
{
	return psBlob.Get();
}

ID3D12PipelineState* Shader::GetPipelineState() const noexcept
{
	return pipelineState.Get();
}

bool Shader::HasPipelineState() const noexcept
{
	return pipelineState != nullptr;
}
