#include "Precompiled.hpp"
#include "Renderer.hpp"

#include <stdexcept>
#include "Buffer.hpp"
#include "GraphicsError.hpp"
#include "RenderTarget.hpp"
#include "Shader.hpp"

namespace TUK::Framework
{
	std::expected<void, std::string> Renderer::Initialize(ID3D12Device& device)
	{
		if (auto result = CheckHResult(device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(commandAllocator.GetAddressOf())), "명령 할당자 생성"); !result)
			return result;
		if (auto result = CheckHResult(device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf())), "명령 목록 생성"); !result)
			return result;
		return CheckHResult(commandList->Close(), "명령 목록 닫기");
	}

	std::expected<void, std::string> Renderer::Begin()
	{
		assert(commandAllocator && commandList && !isRecording);
		if (auto result = CheckHResult(commandAllocator->Reset(), "명령 할당자 초기화"); !result)
			return result;
		if (auto result = CheckHResult(commandList->Reset(commandAllocator.Get(), nullptr), "명령 목록 초기화"); !result)
			return result;
		currentTarget = nullptr;
		hasShader = false;
		isComputeShader = false;
		isRecording = true;
		return {};
	}

	std::expected<void, std::string> Renderer::End()
	{
		(void)GetCommandList();
		auto result = CheckHResult(commandList->Close(), "명령 목록 닫기");
		isRecording = false;
		currentTarget = nullptr;
		return result;
	}

	ID3D12GraphicsCommandList& Renderer::GetCommandList() const
	{
		if (!isRecording || !commandList)
			throw std::logic_error("렌더링 명령은 활성 프레임에서만 기록할 수 있습니다.");
		return *commandList.Get();
	}

	void Renderer::Transition(Texture& texture, D3D12_RESOURCE_STATES nextState)
	{
		auto& commands = GetCommandList();
		if (!texture.resource)
			throw std::logic_error("유효하지 않은 텍스처입니다.");
		if (texture.state == nextState)
			return;
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = texture.resource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = texture.state;
		barrier.Transition.StateAfter = nextState;
		commands.ResourceBarrier(1, &barrier);
		texture.state = nextState;
	}

	void Renderer::SetRenderTarget(RenderTarget& target)
	{
		auto& commands = GetCommandList();
		if (!target.renderViewHeap)
			throw std::logic_error("유효하지 않은 렌더 타겟입니다.");
		Transition(target.color, D3D12_RESOURCE_STATE_RENDER_TARGET);
		auto rtv = target.renderViewHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_CPU_DESCRIPTOR_HANDLE dsv{};
		if (target.depth)
		{
			Transition(*target.depth, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			dsv = target.depthViewHeap->GetCPUDescriptorHandleForHeapStart();
		}
		commands.OMSetRenderTargets(1, &rtv, FALSE, target.depth ? &dsv : nullptr);
		const D3D12_VIEWPORT viewport{ 0.0f, 0.0f, static_cast<float>(target.GetWidth()),
			static_cast<float>(target.GetHeight()), 0.0f, 1.0f };
		const D3D12_RECT scissor{ 0, 0, static_cast<LONG>(target.GetWidth()), static_cast<LONG>(target.GetHeight()) };
		commands.RSSetViewports(1, &viewport);
		commands.RSSetScissorRects(1, &scissor);
		currentTarget = &target;
	}

	void Renderer::Clear(const ColorRGBA<float>& color, float depth)
	{
		auto& commands = GetCommandList();
		if (!currentTarget)
			throw std::logic_error("먼저 렌더 타겟을 연결해야 합니다.");
		const float components[]{ color.GetR(), color.GetG(), color.GetB(), color.GetA() };
		commands.ClearRenderTargetView(currentTarget->renderViewHeap->GetCPUDescriptorHandleForHeapStart(), components, 0, nullptr);
		if (currentTarget->depth)
			commands.ClearDepthStencilView(currentTarget->depthViewHeap->GetCPUDescriptorHandleForHeapStart(),
				D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
	}

	void Renderer::SetShader(const Shader& shader)
	{
		auto& commands = GetCommandList();
		if (!shader.pipelineState || !shader.rootSignature)
			throw std::logic_error("유효하지 않은 셰이더입니다.");
		commands.SetPipelineState(shader.pipelineState.Get());
		if (shader.isCompute)
			commands.SetComputeRootSignature(shader.rootSignature.Get());
		else
			commands.SetGraphicsRootSignature(shader.rootSignature.Get());
		hasShader = true;
		isComputeShader = shader.isCompute;
	}

	void Renderer::SetTexture(UINT parameterIndex, Texture& texture)
	{
		auto& commands = GetCommandList();
		if (!hasShader || !texture.shaderViewHeap || (currentTarget && &texture == &currentTarget->color))
			throw std::logic_error("셰이더와 샘플링 가능한 텍스처가 필요하며 출력 타겟과 같을 수 없습니다.");
		Transition(texture, isComputeShader ? D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE :
			static_cast<D3D12_RESOURCE_STATES>(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		ID3D12DescriptorHeap* heaps[]{ texture.shaderViewHeap.Get() };
		commands.SetDescriptorHeaps(1, heaps);
		auto view = texture.shaderViewHeap->GetGPUDescriptorHandleForHeapStart();
		if (isComputeShader)
			commands.SetComputeRootDescriptorTable(parameterIndex, view);
		else
			commands.SetGraphicsRootDescriptorTable(parameterIndex, view);
	}

	void Renderer::SetVertexBuffer(const Buffer& buffer, UINT stride, UINT slot)
	{
		auto view = buffer.GetVertexBufferView(stride);
		if (!view) throw std::invalid_argument(view.error());
		GetCommandList().IASetVertexBuffers(slot, 1, &*view);
	}

	void Renderer::SetIndexBuffer(const Buffer& buffer, DXGI_FORMAT format)
	{
		auto view = buffer.GetIndexBufferView(format);
		if (!view) throw std::invalid_argument(view.error());
		GetCommandList().IASetIndexBuffer(&*view);
	}

	void Renderer::SetConstants(UINT parameterIndex, std::span<const std::uint32_t> values, UINT offset)
	{
		auto& commands = GetCommandList();
		if (!hasShader) throw std::logic_error("먼저 셰이더를 연결해야 합니다.");
		if (isComputeShader)
			commands.SetComputeRoot32BitConstants(parameterIndex, static_cast<UINT>(values.size()), values.data(), offset);
		else
			commands.SetGraphicsRoot32BitConstants(parameterIndex, static_cast<UINT>(values.size()), values.data(), offset);
	}

	void Renderer::SetConstantBuffer(UINT parameterIndex, const Buffer& buffer)
	{
		auto& commands = GetCommandList();
		if (!hasShader) throw std::logic_error("먼저 셰이더를 연결해야 합니다.");
		if (isComputeShader) commands.SetComputeRootConstantBufferView(parameterIndex, buffer.GetGPUVirtualAddress());
		else commands.SetGraphicsRootConstantBufferView(parameterIndex, buffer.GetGPUVirtualAddress());
	}

	void Renderer::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		GetCommandList().IASetPrimitiveTopology(topology);
	}

	void Renderer::Draw(UINT vertexCount, UINT startVertex, UINT instanceCount)
	{
		if (!currentTarget || !hasShader || isComputeShader) throw std::logic_error("타겟과 그래픽 셰이더가 필요합니다.");
		GetCommandList().DrawInstanced(vertexCount, instanceCount, startVertex, 0);
	}

	void Renderer::DrawIndexed(UINT indexCount, UINT startIndex, INT baseVertex, UINT instanceCount)
	{
		if (!currentTarget || !hasShader || isComputeShader) throw std::logic_error("타겟과 그래픽 셰이더가 필요합니다.");
		GetCommandList().DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, 0);
	}

	void Renderer::Dispatch(UINT x, UINT y, UINT z)
	{
		if (!hasShader || !isComputeShader) throw std::logic_error("컴퓨트 셰이더가 필요합니다.");
		GetCommandList().Dispatch(x, y, z);
	}

	void Renderer::Blit(Texture& source, const Shader& shader)
	{
		SetShader(shader);
		SetTexture(0, source);
		SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Draw(3);
	}

	void Renderer::Release() noexcept
	{
		currentTarget = nullptr;
		isRecording = false;
		hasShader = false;
		commandList.Reset();
		commandAllocator.Reset();
	}
}
