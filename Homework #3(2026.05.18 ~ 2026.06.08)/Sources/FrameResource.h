#pragma once

struct FrameResource final
{
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	UINT64 fenceValue{ 0 };
};
