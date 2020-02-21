#ifndef EVENTS_EVENT_HPP
#define EVENTS_EVENT_HPP

#include "mvar.hpp"

namespace events
{

template <typename T>
class Event
{
public:
    Event() : mvar(std::make_shared<MVar>()) {
    }
    
    Event(const Event &other) : mvar(other.mvar) {
    }
    
    const T& sync() {
        return mvar->read();
    }
    
    // TODO Should be package private.
    void notify(T &&v) {
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
    Event() : mvar(std::make_shared<MVar>()) {
    }
    
    Event(const Event &other) : mvar(other.mvar) {
    }
    
    void sync() {
        mvar->read();
    }
    
    // TODO Should be package private.
    void notify() {
        mvar->tryPut();
    }
    
private:
    using MVar = mvar::MVar<void>;
    using MVarPtr = std::shared_ptr<MVar>;
    
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
