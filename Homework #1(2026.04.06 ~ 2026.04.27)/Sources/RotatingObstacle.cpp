#include "Precompiled.hpp"
#include "RotatingObstacle.hpp"

#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "Collider.hpp"
#include "CollisionBox.hpp"
#include "CollisionSphere.hpp"
#include "Object.hpp"
#include "Particle.hpp"
#include "Quaternion.hpp"
#include "Scene_Game.hpp"
#include "Scene.hpp"
#include "Timer.hpp"
#include "Transform.hpp"
#include "Vector3D.hpp"

namespace
{
	void SpawnExplosionParticles(Scene* scene_, const Vector3D& position_, const ColorRGBA& color_)
	{
		if (!scene_)
		{
			return;
		}

		Mesh* particleMesh = Mesh::LoadFromFile("./Assets/Cube.obj");
		if (!particleMesh)
		{
			particleMesh = Mesh::CreateCube(1.0f, ColorRGB::GetWhite());
		}

		std::mt19937 randomEngine(std::random_device{}());
		std::uniform_real_distribution<float> directionDistribution(-1.0f, 1.0f);
		std::uniform_real_distribution<float> speedDistribution(8.0f, 16.0f);
		std::uniform_real_distribution<float> lifeDistribution(0.35f, 0.8f);
		std::uniform_real_distribution<float> scaleDistribution(0.06f, 0.16f);

		constexpr int particleCount = 24;
		for (int index = 0; index < particleCount; ++index)
		{
			Object* particleObject = scene_->AddObject();
			if (!particleObject)
			{
				continue;
			}

			particleObject->SetName("Particle_" + std::to_string(index));
			particleObject->SetTag("Particle");

			Transform* particleTransform = particleObject->GetTransform();
			if (!particleTransform)
			{
				continue;
			}

			const float scale = scaleDistribution(randomEngine);
			particleTransform->SetWorldPosition(position_);
			particleTransform->SetWorldScale(Vector3D(scale, scale, scale));

			MeshRenderer* meshRenderer = particleObject->AddComponent<MeshRenderer>();
			if (meshRenderer)
			{
				meshRenderer->SetMesh(particleMesh);
				meshRenderer->SetColor(color_);
			}

			Vector3D direction(
				directionDistribution(randomEngine),
				directionDistribution(randomEngine),
				directionDistribution(randomEngine));
			if (direction.IsZero())
			{
				direction = Vector3D::GetUp();
			}
			direction.Normalize();

			Particle* particle = particleObject->AddComponent<Particle>();
			if (particle)
			{
				particle->SetVelocity(direction * speedDistribution(randomEngine));
				particle->SetLifetime(lifeDistribution(randomEngine));
			}
		}
	}
}

void RotatingObstacle::OnAttach()
{
	ChooseNextMovementDirection();
}

float RotatingObstacle::GetRotationSpeed() const
{
	return rotationSpeed;
}

void RotatingObstacle::SetRotationSpeed(float rotationSpeed_)
{
	rotationSpeed = rotationSpeed_;
}

float RotatingObstacle::GetCollisionRadius() const
{
	return collisionRadius;
}

void RotatingObstacle::SetCollisionRadius(float collisionRadius_)
{
	collisionRadius = std::max(collisionRadius_, 0.0f);
}

float RotatingObstacle::GetMovementSpeed() const
{
	return movementSpeed;
}

void RotatingObstacle::SetMovementSpeed(float movementSpeed_)
{
	movementSpeed = std::max(movementSpeed_, 0.0f);
}

void RotatingObstacle::SetDirectionChangeIntervalRange(float minInterval_, float maxInterval_)
{
	minDirectionChangeInterval = std::max(minInterval_, 0.1f);
	maxDirectionChangeInterval = std::max(maxInterval_, minDirectionChangeInterval);
}

int RotatingObstacle::GetObstacleSize() const
{
	return obstacleSize;
}

void RotatingObstacle::SetObstacleSize(int obstacleSize_)
{
	obstacleSize = std::clamp(obstacleSize_, 1, 5);
}

void RotatingObstacle::OnUpdate()
{
	Object* owner = GetOwner();
	Transform* transform = GetTransform();
	if (!owner || !transform)
	{
		return;
	}

	const float deltaTime = Timer::GetDeltaTime();
	const Quaternion deltaRotation = Quaternion::Euler(0.0f, rotationSpeed * Timer::GetDeltaTime(), 0.0f);
	transform->SetWorldRotation(transform->GetWorldRotation() * deltaRotation);

	remainingDirectionChangeTime -= deltaTime;
	if (remainingDirectionChangeTime <= 0.0f)
	{
		ChooseNextMovementDirection();
	}

	const Vector3D nextPosition = transform->GetWorldPosition() + (movementDirection * movementSpeed * deltaTime);
	transform->SetWorldPosition(nextPosition);

	Scene* scene = owner->GetCurrentScene();
	if (!scene)
	{
		return;
	}

	const Vector3D obstaclePosition = transform->GetWorldPosition();
	const Collider* obstacleCollider = owner->GetComponent<CollisionSphere>();
	if (!obstacleCollider)
	{
		obstacleCollider = owner->GetComponent<CollisionBox>();
	}
	Object* hitBullet = nullptr;
	for (const std::unique_ptr<Object>& object : scene->GetObjects())
	{
		Object* target = object.get();
		if (!target || target == owner || target->IsDestroyed() || !target->IsActive())
		{
			continue;
		}

		if (target->GetTag() != "Bullet")
		{
			continue;
		}

		const Collider* bulletCollider = target->GetComponent<CollisionSphere>();
		if (!bulletCollider)
		{
			continue;
		}

		bool isHit = obstacleCollider ? obstacleCollider->Intersects(*bulletCollider) : false;
		if (!isHit)
		{
			const Transform* bulletTransform = target->GetTransform();
			if (bulletTransform)
			{
				const float distance = Vector3D::Distance(obstaclePosition, bulletTransform->GetWorldPosition());
				isHit = (distance <= collisionRadius);
			}
		}

		if (isHit)
		{
			hitBullet = target;
			break;
		}
	}

	if (hitBullet)
	{
		MeshRenderer* obstacleRenderer = owner->GetComponent<MeshRenderer>();
		const ColorRGBA explosionColor = obstacleRenderer ? obstacleRenderer->GetColor() : ColorRGBA::GetWhite();
		const std::size_t gainedScore = static_cast<std::size_t>(obstacleSize * 100);

		SpawnExplosionParticles(scene, obstaclePosition, explosionColor);
		Scene_Game::AddPlayerScore(gainedScore);
		hitBullet->Destroy();
		owner->Destroy();
	}
}

void RotatingObstacle::ChooseNextMovementDirection()
{
	std::uniform_real_distribution<float> directionDistribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> intervalDistribution(minDirectionChangeInterval, maxDirectionChangeInterval);

	Vector3D nextDirection(
		directionDistribution(randomEngine),
		directionDistribution(randomEngine) * 0.35f,
		directionDistribution(randomEngine));

	if (nextDirection.IsZero())
	{
		nextDirection = Vector3D(0.0f, 0.0f, 1.0f);
	}

	nextDirection.Normalize();
	movementDirection = nextDirection;
	remainingDirectionChangeTime = intervalDistribution(randomEngine);
}
