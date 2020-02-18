#ifndef EVENTS_EVENT_HPP
#define EVENTS_EVENT_HPP

#include "mvar.hpp"

namespace events
{

template <typename T>
class Event
{
public:
    Event() {
    }
    
    Event(const Event &other) : mvar(other.mvar) {
    }
    
    T sync() {
        return mvar->take();
    }
    
    // TODO Should be package private.
    void notify(T &&v) {
        mvar->put(std::move(v));
    }
    
private:
    using MVarPtr = std::shared_ptr<mvar::MVar<T>>;
    
    MVarPtr mvar;
};

template <>
class Event<void>
{
public:
    Event() {
    }
    
    Event(const Event &other) : mvar(other.mvar) {
    }
    
    void sync() {
        return mvar->take();
    }
    
    // TODO Should be package private.
    void notify() {
        mvar->put();
    }
    
private:
    using MVarPtr = std::shared_ptr<mvar::MVar<void>>;
    
    MVarPtr mvar;
};

template<typename T>
Event<T> always(T &&v) {
    Event<T> event;
    event.notify(std::move(v));
    
    return event;
}

Event<void> always() {
    Event<void> event;
    event.notify();
    
    return event;
}

template<typename T>
Event<T> never() {
    return Event<T>();
}

template<>
Event<void> never<void>() {
    return Event<void>();
}
    
} // namespace events


#endif  // EVENTS_EVENT_HPP
