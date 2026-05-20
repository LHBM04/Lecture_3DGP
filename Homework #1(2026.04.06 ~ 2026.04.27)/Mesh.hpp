#pragma once

#include "ColorRGB.hpp"
#include "Vector3D.hpp"

class Mesh
{
public:
	struct Vertex final
	{
		Vector3D position;
		ColorRGB color;
	};

	static Mesh* CreateFromData(std::vector<Vertex> vertices_, std::vector<std::uint32_t> indices_);
	static Mesh* LoadFromFile(const std::string& filePath_);

	// 테스트용
	static Mesh* CreateCube(float size_ = 1.0f, const ColorRGB& color_ = ColorRGB::GetWhite());
	
	const std::vector<Vertex>& GetVertices() const;
	const std::vector<std::uint32_t>& GetIndices() const;

private:
	static std::unordered_map<std::string, std::unique_ptr<Mesh>> loadedMeshes;
	static std::uint64_t generatedMeshId;

	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
};
