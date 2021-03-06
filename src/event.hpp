#ifndef EVENTS_EVENT_HPP
#define EVENTS_EVENT_HPP

#include "mvar.hpp"

namespace events
{

template <typename T>
class Event
{
	public:
	Event() : mvar(std::make_shared<MVar>())
	{
	}

	Event(const Event &other) : mvar(other.mvar)
	{
	}

	const T &sync()
	{
		return mvar->read();
	}

	template <typename S>
	Event<S> wrap(std::function<S(const T &)> &&callback)
	{
		Event<S> e;

		std::thread t([this, callback = std::move(callback), &e]() mutable {
			auto result = this->sync();
			e.notify(callback(result));
		});
		t.detach();

		return e;
	}

	Event<T> choose(Event<T> other)
	{
		Event<T> e;

		std::thread t1([this, &e]() mutable {
			auto result = this->sync();
			e.notify(std::move(result));
		});
		t1.detach();

		std::thread t2([other, &e]() mutable {
			auto result = other.sync();
			e.notify(std::move(result));
		});
		t2.detach();

		return e;
	}

	// TODO Should be package private.
	void notify(T &&v)
	{
		mvar->tryPut(std::move(v));
	}

	private:
	using MVar = mvar::MVar<T>;
	using MVarPtr = std::shared_ptr<MVar>;

	MVarPtr mvar;
};

template <>
class Event<void>
{
	public:
	Event() : mvar(std::make_shared<MVar>())
	{
	}

	Event(const Event &other) : mvar(other.mvar)
	{
	}

	void sync()
	{
		mvar->read();
	}

	template <typename S>
	Event<S> wrap(std::function<S()> &&callback)
	{
		Event<S> e;

		std::thread t([this, callback = std::move(callback), &e]() mutable {
			this->sync();
			e.notify(callback());
		});
		t.detach();

		return e;
	}

	Event<void> choose(Event<void> other)
	{
		Event<void> e;

		std::thread t1([this, &e]() mutable {
			this->sync();
			e.notify();
		});
		t1.detach();

		std::thread t2([other, &e]() mutable {
			other.sync();
			e.notify();
		});
		t2.detach();

		return e;
	}

	// TODO Should be package private.
	void notify()
	{
		mvar->tryPut();
	}

	private:
	using MVar = mvar::MVar<void>;
	using MVarPtr = std::shared_ptr<MVar>;

	MVarPtr mvar;
};

template <typename T>
Event<T> always(T &&v)
{
	Event<T> event;
	event.notify(std::move(v));

	return event;
}

Event<void> always()
{
	Event<void> event;
	event.notify();

	return event;
}

template <typename T>
Event<T> never()
{
	return Event<T>();
}

template <>
Event<void> never<void>()
{
	return Event<void>();
}

} // namespace events

#endif // EVENTS_EVENT_HPP
