#pragma once

class GameObject;

class ICollidable
{
public:
	virtual ~ICollidable() = default;

	virtual void OnCollisionEnter(GameObject& other_) = 0;
	virtual void OnCollisionStay(GameObject& other_) = 0;
	virtual void OnCollisionExit(GameObject& other_) = 0;
};
