#include "Precompiled.h"
#include "Mesh.h"

namespace
{
	struct FaceVertex final
	{
		int position{ 0 };
		int texCoord{ 0 };
		int normal{ 0 };
	};

	FaceVertex ParseFaceVertex(const std::string& token_)
	{
		FaceVertex result;
		std::stringstream stream{ token_ };
		std::string part;

		if (std::getline(stream, part, '/') && !part.empty())
		{
			result.position = std::stoi(part);
		}

		if (std::getline(stream, part, '/') && !part.empty())
		{
			result.texCoord = std::stoi(part);
		}

		if (std::getline(stream, part, '/') && !part.empty())
		{
			result.normal = std::stoi(part);
		}

		return result;
	}

	template <class TValue>
	const TValue* ResolveOBJIndex(const std::vector<TValue>& values_, int index_)
	{
		if (index_ > 0)
		{
			const std::size_t resolved{ static_cast<std::size_t>(index_ - 1) };
			return resolved < values_.size() ? &values_[resolved] : nullptr;
		}

		if (index_ < 0)
		{
			const std::ptrdiff_t resolved{ static_cast<std::ptrdiff_t>(values_.size()) + index_ };
			return resolved >= 0 ? &values_[static_cast<std::size_t>(resolved)] : nullptr;
		}

		return nullptr;
	}
}

bool Mesh::Load(const std::filesystem::path& path_)
{
	Unload();

	if (path_.extension() == ".obj")
	{
		return LoadOBJ(path_);
	}

	return false;
}

void Mesh::Unload() noexcept
{
	ClearPath();
	SetLoaded(false);
	vertices.clear();
	indices.clear();
}

const std::vector<MeshVertex>& Mesh::GetVertices() const noexcept
{
	return vertices;
}

const std::vector<std::uint32_t>& Mesh::GetIndices() const noexcept
{
	return indices;
}

void Mesh::SetData(std::vector<MeshVertex> vertices_, std::vector<std::uint32_t> indices_)
{
	ClearPath();
	vertices = std::move(vertices_);
	indices = std::move(indices_);
	SetLoaded(!vertices.empty());
}

bool Mesh::LoadOBJ(const std::filesystem::path& path_)
{
	std::ifstream file{ path_ };
	if (not file)
	{
		return false;
	}

	std::vector<Vector3D> positions;
	std::vector<Vector3D> normals;
	std::vector<Vector2D> texCoords;

	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream stream{ line };
		std::string type;
		stream >> type;

		if (type == "v")
		{
			float x;
			float y;
			float z;
			stream >> x >> y >> z;
			positions.emplace_back(x, y, z);
		}
		else if (type == "vn")
		{
			float x;
			float y;
			float z;
			stream >> x >> y >> z;
			normals.emplace_back(x, y, z);
		}
		else if (type == "vt")
		{
			float u;
			float v;
			stream >> u >> v;
			texCoords.emplace_back(u, v);
		}
		else if (type == "f")
		{
			std::vector<FaceVertex> face;
			std::string token;
			while (stream >> token)
			{
				face.emplace_back(ParseFaceVertex(token));
			}

			if (face.size() < 3)
			{
				continue;
			}

			for (std::size_t index{ 1 }; index + 1 < face.size(); ++index)
			{
				const FaceVertex triangle[3]{ face[0], face[index], face[index + 1] };
				for (const FaceVertex& faceVertex : triangle)
				{
					const Vector3D* position{ ResolveOBJIndex(positions, faceVertex.position) };
					if (nullptr == position)
					{
						return false;
					}

					MeshVertex vertex{};
					vertex.position = *position;

					if (const Vector3D* normal{ ResolveOBJIndex(normals, faceVertex.normal) })
					{
						vertex.normal = *normal;
					}

					if (const Vector2D* texCoord{ ResolveOBJIndex(texCoords, faceVertex.texCoord) })
					{
						vertex.texCoord = *texCoord;
					}

					vertices.emplace_back(vertex);
					indices.emplace_back(static_cast<std::uint32_t>(indices.size()));
				}
			}
		}
	}

	if (vertices.empty())
	{
		Unload();
		return false;
	}

	SetPath(path_);
	SetLoaded(true);
	return true;
}
