#pragma once

class Event
{
public:
	explicit Event() = default;
	virtual ~Event() noexcept = default;

	Event(const Event&) = default;
	Event& operator=(const Event&) = default;

	Event(Event&&) noexcept = default;
	Event& operator=(Event&&) noexcept = default;

	template <std::derived_from<Event> TEvent>
	[[nodiscard]] inline bool Is() const noexcept;

	template <std::derived_from<Event> TEvent>
	[[nodiscard]] inline TEvent& As() noexcept;

	template <std::derived_from<Event> TEvent>
	[[nodiscard]] inline const TEvent& As() const noexcept;

	[[nodiscard]] bool IsHandled() const noexcept;
	void SetHandled(bool value_) noexcept;

private:
	bool handled{ false };
};

template <std::derived_from<Event> TEvent>
inline bool Event::Is() const noexcept
{
	return typeid(*this) == typeid(TEvent);
}

template <std::derived_from<Event> TEvent>
inline TEvent& Event::As() noexcept
{
	assert(Is<TEvent>());
	return static_cast<TEvent&>(*this);
}

template <std::derived_from<Event> TEvent>
inline const TEvent& Event::As() const noexcept
{
	assert(Is<TEvent>());
	return static_cast<const TEvent&>(*this);
}
