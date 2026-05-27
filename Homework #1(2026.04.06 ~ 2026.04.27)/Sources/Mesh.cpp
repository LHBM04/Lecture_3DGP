#include "Precompiled.hpp"
#include "Mesh.hpp"

#include "Logger.hpp"

const std::vector<Mesh::Vertex>& Mesh::GetVertices() const
{
	return vertices;
}

const std::vector<std::uint32_t>& Mesh::GetIndices() const
{
	return indices;
}

Mesh* Mesh::CreateFromData(std::vector<Vertex> vertices_, std::vector<std::uint32_t> indices_)
{
	auto mesh = std::make_unique<Mesh>();
	mesh->vertices = std::move(vertices_);
	mesh->indices = std::move(indices_);

	const std::string meshKey = "__generated_mesh_" + std::to_string(generatedMeshId++);
	Mesh* meshPtr = mesh.get();
	loadedMeshes[meshKey] = std::move(mesh);
	return meshPtr;
}

Mesh* Mesh::CreateCube(float size_, const ColorRGB& color_)
{
	const float halfSize = size_ * 0.5f;

	std::vector<Vertex> cubeVertices
	{
		{ Vector3D(-halfSize, -halfSize, -halfSize), color_ },
		{ Vector3D( halfSize, -halfSize, -halfSize), color_ },
		{ Vector3D( halfSize,  halfSize, -halfSize), color_ },
		{ Vector3D(-halfSize,  halfSize, -halfSize), color_ },
		{ Vector3D(-halfSize, -halfSize,  halfSize), color_ },
		{ Vector3D( halfSize, -halfSize,  halfSize), color_ },
		{ Vector3D( halfSize,  halfSize,  halfSize), color_ },
		{ Vector3D(-halfSize,  halfSize,  halfSize), color_ }
	};

	std::vector<std::uint32_t> cubeIndices
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		0, 4, 5, 0, 5, 1,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		0, 3, 7, 0, 7, 4
	};

	return CreateFromData(std::move(cubeVertices), std::move(cubeIndices));
}

Mesh* Mesh::LoadFromFile(const std::string& filePath_)
{
	std::ifstream file;
	std::filesystem::path resolvedPath;
	{
		const std::filesystem::path inputPath = std::filesystem::path(filePath_);
		std::vector<std::filesystem::path> candidates;
		candidates.push_back(inputPath);

		const std::filesystem::path cwd = std::filesystem::current_path();
		if (inputPath.is_relative())
		{
			candidates.push_back(cwd / inputPath);
			candidates.push_back(cwd / "Assets" / inputPath.filename());
			candidates.push_back(cwd.parent_path() / "Assets" / inputPath.filename());
			candidates.push_back(cwd.parent_path().parent_path() / "Assets" / inputPath.filename());
			candidates.push_back(cwd.parent_path().parent_path().parent_path() / "Assets" / inputPath.filename());
		}

		for (const std::filesystem::path& candidate : candidates)
		{
			std::ifstream candidateFile(candidate);
			if (candidateFile)
			{
				resolvedPath = candidate;
				file = std::move(candidateFile);
				break;
			}
		}
	}

	if (!file)
	{
		Logger::Error("Mesh file not found: %s", filePath_.c_str());
		return nullptr;
	}

	std::string cacheKey;
	try
	{
		cacheKey = std::filesystem::weakly_canonical(resolvedPath).string();
	}
	catch (...)
	{
		try
		{
			cacheKey = std::filesystem::absolute(resolvedPath).lexically_normal().string();
		}
		catch (...)
		{
			cacheKey = resolvedPath.lexically_normal().string();
		}
	}

	const auto cachedIterator = loadedMeshes.find(cacheKey);
	if (cachedIterator != loadedMeshes.end())
	{
		return cachedIterator->second.get();
	}

	auto mesh = std::make_unique<Mesh>();

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
		{
			continue;
		}

		std::istringstream stream(line);
		std::string token;
		stream >> token;

		if (token == "v")
		{
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			stream >> x >> y >> z;

			float r = 1.0f;
			float g = 1.0f;
			float b = 1.0f;
			if (stream >> r >> g >> b)
			{
				mesh->vertices.push_back(Vertex{ Vector3D(x, y, z), ColorRGB(r, g, b) });
			}
			else
			{
				mesh->vertices.push_back(Vertex{ Vector3D(x, y, z), ColorRGB::GetWhite() });
			}
		}
		else if (token == "f")
		{
			std::vector<std::uint32_t> polygonIndices;
			std::string indexToken;
			while (stream >> indexToken)
			{
				const std::size_t slashPosition = indexToken.find('/');
				const std::string vertexToken =
					(slashPosition == std::string::npos) ? indexToken : indexToken.substr(0, slashPosition);
				if (vertexToken.empty())
				{
					polygonIndices.clear();
					break;
				}

				int objIndex = 0;
				try
				{
					objIndex = std::stoi(vertexToken);
				}
				catch (...)
				{
					polygonIndices.clear();
					break;
				}

				if (objIndex == 0)
				{
					polygonIndices.clear();
					break;
				}

				long long resolvedIndex = 0;
				if (objIndex > 0)
				{
					resolvedIndex = static_cast<long long>(objIndex - 1);
				}
				else
				{
					resolvedIndex = static_cast<long long>(mesh->vertices.size()) + static_cast<long long>(objIndex);
				}

				if (resolvedIndex < 0 || resolvedIndex >= static_cast<long long>(mesh->vertices.size()))
				{
					polygonIndices.clear();
					break;
				}

				polygonIndices.push_back(static_cast<std::uint32_t>(resolvedIndex));
			}

			if (polygonIndices.size() < 3)
			{
				continue;
			}

			for (std::size_t index = 1; index + 1 < polygonIndices.size(); ++index)
			{
				mesh->indices.push_back(polygonIndices[0]);
				mesh->indices.push_back(polygonIndices[index]);
				mesh->indices.push_back(polygonIndices[index + 1]);
			}
		}
	}

	if (mesh->vertices.empty())
	{
		Logger::Error("Mesh file has no vertices: %s", resolvedPath.string().c_str());
		return nullptr;
	}

	if (!mesh->indices.empty())
	{
		const std::uint32_t maxIndex = *std::max_element(mesh->indices.begin(), mesh->indices.end());
		if (maxIndex >= mesh->vertices.size())
		{
			Logger::Error("Mesh file has invalid index range: %s", resolvedPath.string().c_str());
			return nullptr;
		}
	}

	Mesh* meshPtr = mesh.get();
	loadedMeshes[cacheKey] = std::move(mesh);
	Logger::Info("Mesh loaded: %s", resolvedPath.string().c_str());
	return meshPtr;
}

std::unordered_map<std::string, std::unique_ptr<Mesh>> Mesh::loadedMeshes;
std::uint64_t Mesh::generatedMeshId = 0;
