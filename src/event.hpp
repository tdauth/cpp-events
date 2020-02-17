#ifndef EVENTS_EVENT_HPP
#define EVENTS_EVENT_HPP

#include "mvar.hpp"

namespace events
{

template <typename T>
class Event
{
public:
    Event();
    T sync() {
        return mvar.take();
    }
    
    void sync(T &&v) {
        mvar.put(std::move(v));
    }
    
private:
    mvar::MVar<T> mvar;
};

template <>
class Event<void>
{
public:
    Event();
    void syncReceive() {
        return mvar.take();
    }
    
    void syncSend() {
        mvar.put();
    }
    
private:
    mvar::MVar<void> mvar;
};
    
} // namespace events


#endif  // EVENTS_EVENT_HPP
