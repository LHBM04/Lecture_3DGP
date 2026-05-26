#pragma once

class GameObject;

class Collidable
{
public:
	virtual ~Collidable() = default;
	virtual void OnCollisionEnter(GameObject& other_) = 0;
	virtual void OnCollisionStay(GameObject& other_) = 0;
	virtual void OnCollisionExit(GameObject& other_) = 0;
};
