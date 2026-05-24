#pragma once

#include "ColorRGBA.h"
#include "Matrix4x4.h"

#include <d3d12.h>
#include <cstdint>

struct DrawCall final
{
	ID3D12PipelineState* pipelineState;
	ID3D12RootSignature* graphicsRootSignature;

	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	bool hasIndexBuffer;
	bool indexed;

	D3D12_GPU_DESCRIPTOR_HANDLE materialDescriptorTable;
	ColorRGBA materialColor;

	UINT vertexCount;
	UINT startVertexLocation;

	UINT indexCount;
	UINT startIndexLocation;
	INT baseVertexLocation;

	UINT instanceCount;
	UINT startInstanceLocation;

	uint64_t pipelineId;
	uint64_t materialId;
	uint64_t meshId;

	Matrix4x4 worldTransform;

	uint64_t sortKey;
};
