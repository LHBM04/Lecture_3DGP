#pragma once

class Object;
class Transform;

class Component
{
public:
	virtual ~Component() = default;

	[[nodiscard]] bool IsAttached() const;

	[[nodiscard]] bool IsEnabled() const;
	void SetEnabled(bool enabled_);

	[[nodiscard]] bool IsDestroyed() const;
	void Destroy();

	[[nodiscard]] Object* GetOwner() const;
	[[nodiscard]] Transform* GetTransform() const;

	void Attach(Object* owner_);
	void Update();
	void Render();
	void Detach();

protected:
	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnLateUpdate() {}
	virtual void OnPreRender() {}
	virtual void OnRender() {}

private:
	bool isEnabled{ true };
	bool isDestroyed{ false };

	Object* owner{ nullptr };
};