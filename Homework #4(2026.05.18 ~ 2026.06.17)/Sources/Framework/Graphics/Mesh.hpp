#pragma once

namespace TUK::Framework
{
	class Mesh
	{
	public:
		explicit Mesh(ID3D12Resource2* vertexBuffer, ID3D12Resource2* indexBuffer) noexcept;
		~Mesh() noexcept;

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		
		Mesh(Mesh&&) noexcept;
		Mesh& operator=(Mesh&&) noexcept;

	private:
		ID3D12Resource2* vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		ID3D12Resource2* indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};
}
