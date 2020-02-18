#ifndef EVENTS_CHANNEL_HPP
#define EVENTS_CHANNEL_HPP

#include <thread>

#include "event.hpp"

namespace events
{

template <typename T>
class Channel
{
public:
    Channel() {
    }
    
    Event<void> send(T &&v) {
        SenderPtr sender = std::make_shared<Sender>(std::move(v));
        
        std::thread t([sender, this]() mutable {
            {
                std::lock_guard<std::mutex> l(this->sendersMutex);
                senders.push_back(sender);
            }
            
            this->m.put(std::move(sender)); // TODO Allow a copy constructor here
        });
        
        return sender->e;
    }
    
    Event<T> receive() {
        ReceiverPtr receiver = std::make_shared<Receiver>();
         
        std::thread t([receiver, this]() mutable {
            {
                std::lock_guard<std::mutex> l(this->receiversMutex);
                receivers.push_back(receiver);
            }
            
            SenderPtr sender = this->m.take();
            receiver->e.notify(std::move(sender->value));
            sender->e.notify();
        });
        
        return receiver->e;
    }
    
    void close() {
        for (SenderPtr &sender : senders) {
            sender.e.sync(); // TODO How to notify about closing?
        }
        
        for (ReceiverPtr &receiver : receivers) {
            receiver.e.sync(); // TODO How to notify about closing?
        }
        
        {
            std::lock_guard<std::mutex> l(this->sendersMutex);
            senders.clear();
        }
        
        {
            std::lock_guard<std::mutex> l(this->receiversMutex);
            receivers.clear();
        }
    }
    
private:
    struct Sender {
        Sender(T &&value) : value(std::move(value)) {
        }
        Sender(Sender&& m) = default;
        
        Event<void> e;
        T value;
    };
    
    struct Receiver {
        Receiver() {
        }
        
        Receiver(Receiver&& m) = default;
        
        Event<T> e;
    };
    
    using SenderPtr = std::shared_ptr<Sender>;
    using ReceiverPtr = std::shared_ptr<Receiver>;
    
    mvar::MVar<SenderPtr> m;
    std::mutex sendersMutex;
    std::vector<SenderPtr> senders;
    std::mutex receiversMutex;
    std::vector<ReceiverPtr> receivers;
};
    
} // namespace events


#endif  // EVENTS_CHANNEL_HPP
