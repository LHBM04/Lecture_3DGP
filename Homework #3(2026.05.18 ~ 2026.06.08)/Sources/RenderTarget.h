#pragma once

class RenderTarget final
{
public:
	RenderTarget() = default;

	void Initialize(
		Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
		D3D12_RESOURCE_STATES initialState) noexcept;
	[[nodiscard]] ID3D12Resource* GetResource() const noexcept;
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const noexcept;
	[[nodiscard]] D3D12_RESOURCE_STATES GetState() const noexcept;

	void Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES nextState);
	void Bind(ID3D12GraphicsCommandList* commandList) const;
	void Clear(ID3D12GraphicsCommandList* commandList, const FLOAT clearColor[4]) const;
	void Reset() noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
	D3D12_RESOURCE_STATES state{ D3D12_RESOURCE_STATE_COMMON };
};
