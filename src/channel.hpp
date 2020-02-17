#ifndef EVENTS_CHANNEL_HPP
#define EVENTS_CHANNEL_HPP

#include <vector>

#include "event.hpp"

namespace events
{

template <typename T>
class Channel
{
public:
    /**
     * @param bufferSize The channel is unbuffered if this value is 0.
     */
    Channel(std::size_t bufferSize) : buffer(bufferSize) {
    }
        
    
    /**
     * Writes the value into the channel.
     */
    Event<void> send(T &&v);
    Event<T> receive();
    void close();
    
private:
    std::vector<T> buffer;
};
    
} // namespace events


#endif  // EVENTS_CHANNEL_HPP
