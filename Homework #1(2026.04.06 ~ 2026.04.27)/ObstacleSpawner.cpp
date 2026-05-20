#include "Precompiled.hpp"
#include "ObstacleSpawner.hpp"

#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "CollisionBox.hpp"
#include "CollisionSphere.hpp"
#include "Object.hpp"
#include "Quaternion.hpp"
#include "RotatingObstacle.hpp"
#include "Scene.hpp"
#include "Transform.hpp"
#include "Vector3D.hpp"

int ObstacleSpawner::GetMaxObstacleCount() const
{
	return maxObstacleCount;
}

void ObstacleSpawner::SetMaxObstacleCount(int maxObstacleCount_)
{
	maxObstacleCount = std::max(maxObstacleCount_, 0);
}

void ObstacleSpawner::OnAttach()
{
	const int neededCount = GetDesiredObstacleCount() - GetCurrentObstacleCount();
	if (neededCount > 0)
	{
		SpawnObstacles(neededCount);
	}
}

void ObstacleSpawner::OnUpdate()
{
	const int neededCount = GetDesiredObstacleCount() - GetCurrentObstacleCount();
	if (neededCount > 0)
	{
		SpawnObstacles(neededCount);
	}
}

void ObstacleSpawner::SpawnObstacles(int count_)
{
	Object* owner = GetOwner();
	Transform* ownerTransform = GetTransform();
	if (!owner || !ownerTransform || count_ <= 0)
	{
		return;
	}

	Scene* scene = owner->GetCurrentScene();
	if (!scene)
	{
		return;
	}

	const int spawnCount = std::min(count_, GetDesiredObstacleCount());
	if (spawnCount <= 0)
	{
		return;
	}

	Mesh* mesh = GetOrCreateObstacleMesh();
	if (!mesh)
	{
		return;
	}

	std::uniform_real_distribution<float> angleDistribution(0.0f, 360.0f);
	std::uniform_real_distribution<float> radiusDistribution(minSpawnRadius, maxSpawnRadius);
	std::uniform_real_distribution<float> heightDistribution(-5.0f, 5.0f);
	std::uniform_int_distribution<int> scaleDistribution(
		static_cast<int>(std::round(minObstacleScale)),
		static_cast<int>(std::round(maxObstacleScale)));
	std::uniform_real_distribution<float> rotationSpeedDistribution(minRotationSpeed, maxRotationSpeed);
	std::uniform_real_distribution<float> movementSpeedDistribution(1.5f, 4.5f);
	std::uniform_real_distribution<float> directionChangeMinDistribution(1.0f, 2.0f);
	std::uniform_real_distribution<float> directionChangeMaxDistribution(2.5f, 4.5f);

	const Vector3D centerPosition = ownerTransform->GetWorldPosition();

	for (int index = 0; index < spawnCount; ++index)
	{
		const float angleDegree = angleDistribution(randomEngine);
		const float angleRadian = angleDegree * Mathf::DEG2RAD;
		const float radius = radiusDistribution(randomEngine);
		const int obstacleSize = scaleDistribution(randomEngine);
		const float scale = static_cast<float>(obstacleSize);
		const float rotationSpeed = rotationSpeedDistribution(randomEngine);
		const float movementSpeed = movementSpeedDistribution(randomEngine);
		const float directionChangeMinInterval = directionChangeMinDistribution(randomEngine);
		const float directionChangeMaxInterval = std::max(directionChangeMaxDistribution(randomEngine), directionChangeMinInterval + 0.25f);

		Object* obstacleObject = scene->AddObject();
		if (!obstacleObject)
		{
			continue;
		}

		obstacleObject->SetName("Obstacle_" + std::to_string(spawnSerial++));
		obstacleObject->SetTag("Obstacle");

		Transform* obstacleTransform = obstacleObject->GetTransform();
		if (!obstacleTransform)
		{
			continue;
		}

		const Vector3D spawnPosition(
			centerPosition.x + std::cos(angleRadian) * radius,
			centerPosition.y + heightDistribution(randomEngine),
			centerPosition.z + std::sin(angleRadian) * radius);

		obstacleTransform->SetWorldPosition(spawnPosition);
		obstacleTransform->SetWorldRotation(Quaternion::Euler(0.0f, angleDegree, 0.0f));
		obstacleTransform->SetWorldScale(Vector3D(scale, scale, scale));

		MeshRenderer* meshRenderer = obstacleObject->AddComponent<MeshRenderer>();
		if (meshRenderer)
		{
			meshRenderer->SetColor(ColorRGBA::GetWhite());
			meshRenderer->SetMesh(mesh);
		}

		RotatingObstacle* rotatingObstacle = obstacleObject->AddComponent<RotatingObstacle>();
		if (rotatingObstacle)
		{
			rotatingObstacle->SetRotationSpeed(rotationSpeed);
			rotatingObstacle->SetCollisionRadius(scale * 0.75f);
			rotatingObstacle->SetMovementSpeed(movementSpeed);
			rotatingObstacle->SetDirectionChangeIntervalRange(directionChangeMinInterval, directionChangeMaxInterval);
			rotatingObstacle->SetObstacleSize(obstacleSize);
		}

		CollisionSphere* collisionSphere = obstacleObject->AddComponent<CollisionSphere>();
		if (collisionSphere)
		{
			collisionSphere->SetLocalCenter(Vector3D::GetZero());
			collisionSphere->SetLocalRadius(0.75f);
		}

		CollisionBox* collisionBox = obstacleObject->AddComponent<CollisionBox>();
		if (collisionBox)
		{
			collisionBox->SetLocalCenter(Vector3D::GetZero());
			collisionBox->SetLocalExtents(Vector3D(0.5f, 0.5f, 0.5f));
		}
	}
}

int ObstacleSpawner::GetCurrentObstacleCount() const
{
	const Object* owner = GetOwner();
	if (!owner)
	{
		return 0;
	}

	const Scene* scene = owner->GetCurrentScene();
	if (!scene)
	{
		return 0;
	}

	int count = 0;
	for (const std::unique_ptr<Object>& object : scene->GetObjects())
	{
		const Object* candidate = object.get();
		if (!candidate || candidate->IsDestroyed())
		{
			continue;
		}

		if (candidate->GetTag() == "Obstacle")
		{
			++count;
		}
	}

	return count;
}

int ObstacleSpawner::GetDesiredObstacleCount() const
{
	return std::clamp(maxObstacleCount, 0, 50);
}

Mesh* ObstacleSpawner::GetOrCreateObstacleMesh()
{
	if (obstacleMesh)
	{
		return obstacleMesh;
	}

	obstacleMesh = Mesh::LoadFromFile("./Assets/Cube.obj");
	if (!obstacleMesh)
	{
		obstacleMesh = Mesh::CreateCube(1.0f, ColorRGB::GetWhite());
	}

	return obstacleMesh;
}
