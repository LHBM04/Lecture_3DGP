#include "Precompiled.h"
#include "Material.h"

#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "Shader.h"

Material::Material(std::wstring_view resourceName_, std::filesystem::path sourcePath_)
	: Resource(resourceName_, std::move(sourcePath_))
{
}

bool Material::Load()
{
	if (IsLoaded())
	{
		return true;
	}

	if (!GetPath().empty() && GetPath().extension() == L".bin")
	{
		std::ifstream file(GetPath(), std::ios::binary);
		if (!file.is_open())
		{
			return false;
		}

		if (!ReadTag(file, "<Material>:"))
		{
			return false;
		}

		if (ReadTag(file, "<BaseColor>:"))
		{
			file.read(reinterpret_cast<char*>(&color), sizeof(ColorRGBA));
		}
		else
		{
			file.clear();
			file.seekg(0, std::ios::beg);
			if (!ReadTag(file, "<Material>:"))
			{
				return false;
			}
		}

		if (ReadTag(file, "<Shader>:"))
		{
			const std::wstring shaderPath{ ReadString(file) };
			shader = ResourceSystem::GetInstance().GetResource<Shader>(shaderPath);
		}
	}

	if (inputElements.empty())
	{
		const std::array<InputElement, 2> defaultInputLayout
		{
			InputElement{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 },
			InputElement{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 }
		};
		SetInputLayout(defaultInputLayout);
	}

	return BuildPipelineState();
}

void Material::Unload()
{
	rootSignature.Reset();
	pipelineState.Reset();
	serializedRootSignature.Reset();
	rootSignatureErrors.Reset();
	MarkLoaded(false);
}

void Material::SetShader(Shader* shader_) noexcept
{
	shader = shader_;
	MarkLoaded(false);
}

Shader* Material::GetShader() noexcept
{
	return shader;
}

const Shader* Material::GetShader() const noexcept
{
	return shader;
}

void Material::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

const ColorRGBA& Material::GetColor() const noexcept
{
	return color;
}

void Material::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_) noexcept
{
	primitiveTopology = primitiveTopology_;
}

D3D12_PRIMITIVE_TOPOLOGY Material::GetPrimitiveTopology() const noexcept
{
	return primitiveTopology;
}

void Material::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType_) noexcept
{
	primitiveTopologyType = primitiveTopologyType_;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE Material::GetPrimitiveTopologyType() const noexcept
{
	return primitiveTopologyType;
}

void Material::SetFillMode(D3D12_FILL_MODE fillMode_) noexcept
{
	fillMode = fillMode_;
	MarkLoaded(false);
}

void Material::SetCullMode(D3D12_CULL_MODE cullMode_) noexcept
{
	cullMode = cullMode_;
	MarkLoaded(false);
}

void Material::SetDepthEnabled(bool isEnabled_) noexcept
{
	isDepthEnabled = isEnabled_;
	MarkLoaded(false);
}

void Material::SetBlendEnabled(bool isEnabled_) noexcept
{
	isBlendEnabled = isEnabled_;
	MarkLoaded(false);
}

void Material::SetInputLayout(std::span<const InputElement> inputElements_)
{
	inputElements.assign(inputElements_.begin(), inputElements_.end());
	MarkLoaded(false);
}

const std::vector<Material::InputElement>& Material::GetInputLayout() const noexcept
{
	return inputElements;
}

bool Material::BuildPipelineState(DXGI_FORMAT renderTargetFormat_, DXGI_FORMAT depthStencilFormat_)
{
	Unload();

	ID3D12Device* const device{ RenderSystem::GetInstance().GetDevice() };
	if (device == nullptr || shader == nullptr || !shader->IsLoaded())
	{
		return false;
	}

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	D3D12_ROOT_PARAMETER rootParameters[4]{};
	for (UINT rootParameterIndex{ 0 }; rootParameterIndex < 4; ++rootParameterIndex)
	{
		rootParameters[rootParameterIndex].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[rootParameterIndex].Descriptor.ShaderRegister = rootParameterIndex;
		rootParameters[rootParameterIndex].Descriptor.RegisterSpace = 0;
		rootParameters[rootParameterIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}
	rootSignatureDesc.NumParameters = 4;
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (FAILED(::D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSignature,
		&rootSignatureErrors)))
	{
		return false;
	}

	if (FAILED(device->CreateRootSignature(
		0,
		serializedRootSignature->GetBufferPointer(),
		serializedRootSignature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature))))
	{
		Unload();
		return false;
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDescs;
	inputLayoutDescs.reserve(inputElements.size());
	for (const InputElement& inputElement : inputElements)
	{
		D3D12_INPUT_ELEMENT_DESC desc{};
		desc.SemanticName = inputElement.semanticName.c_str();
		desc.SemanticIndex = inputElement.semanticIndex;
		desc.Format = inputElement.format;
		desc.InputSlot = inputElement.inputSlot;
		desc.AlignedByteOffset = inputElement.alignedByteOffset;
		desc.InputSlotClass = inputElement.inputSlotClass;
		desc.InstanceDataStepRate = inputElement.instanceDataStepRate;
		inputLayoutDescs.push_back(desc);
	}

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
	if (isBlendEnabled)
	{
		D3D12_RENDER_TARGET_BLEND_DESC& renderTargetBlendDesc{ blendDesc.RenderTarget[0] };
		renderTargetBlendDesc.BlendEnable = TRUE;
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	}

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = fillMode;
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = isDepthEnabled ? TRUE : FALSE;
	depthStencilDesc.DepthWriteMask = isDepthEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.pRootSignature = rootSignature.Get();
	pipelineStateDesc.VS = shader->GetVertexShaderBytecode();
	pipelineStateDesc.PS = shader->GetPixelShaderBytecode();
	pipelineStateDesc.BlendState = blendDesc;
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.RasterizerState = rasterizerDesc;
	pipelineStateDesc.DepthStencilState = depthStencilDesc;
	pipelineStateDesc.InputLayout = { inputLayoutDescs.data(), static_cast<UINT>(inputLayoutDescs.size()) };
	pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pipelineStateDesc.PrimitiveTopologyType = primitiveTopologyType;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = renderTargetFormat_;
	pipelineStateDesc.DSVFormat = depthStencilFormat_;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0;

	if (FAILED(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState))))
	{
		Unload();
		return false;
	}

	MarkLoaded(true);
	return true;
}

void Material::Bind(ID3D12GraphicsCommandList* commandList_) const
{
	if (commandList_ == nullptr || !IsLoaded())
	{
		return;
	}

	commandList_->SetGraphicsRootSignature(rootSignature.Get());
	commandList_->SetPipelineState(pipelineState.Get());
	commandList_->IASetPrimitiveTopology(primitiveTopology);
}

MaterialConstants Material::GetMaterialConstants() const noexcept
{
	MaterialConstants constants{};
	constants.albedoColor = color;
	return constants;
}

ID3D12RootSignature* Material::GetRootSignature() const noexcept
{
	return rootSignature.Get();
}

ID3D12PipelineState* Material::GetPipelineState() const noexcept
{
	return pipelineState.Get();
}

bool Material::ReadTag(std::ifstream& file_, const std::string& expectedTag_)
{
	uint8_t tagLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&tagLength), sizeof(tagLength))))
	{
		return false;
	}

	std::string tag(tagLength, '\0');
	if (!static_cast<bool>(file_.read(tag.data(), tagLength)))
	{
		return false;
	}

	return tag == expectedTag_;
}

std::wstring Material::ReadString(std::ifstream& file_)
{
	uint8_t stringLength{ 0 };
	if (!static_cast<bool>(file_.read(reinterpret_cast<char*>(&stringLength), sizeof(stringLength))))
	{
		return {};
	}

	std::string value(stringLength, '\0');
	if (!static_cast<bool>(file_.read(value.data(), stringLength)))
	{
		return {};
	}

	return std::wstring(value.begin(), value.end());
}
