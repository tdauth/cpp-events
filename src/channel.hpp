#ifndef EVENTS_CHANNEL_HPP
#define EVENTS_CHANNEL_HPP

#include <thread>
#include <optional>

#include "event.hpp"

namespace events
{

template <typename T>
class Channel
{
public:
    /**
     * If the result is false it means that the channel has been closed.
     */
    using SendResult = bool;
    /**
     * If the result is not set it means that the channel has been closed.
     */
    using ReceiveResult = std::optional<T>;
    
    Channel() {
    }
    
    virtual ~Channel() {
        close();
    }
    
    Channel(Channel &&) = default;
    Channel(const Channel &) = delete;
    Channel& operator=(const Channel &) = delete;
    
    SendResult send(T &&v) {
        return sendEvt(std::move(v)).sync();
    }
    
    ReceiveResult recv() {
        return recvEvt().sync();
    }
    
    Event<SendResult> sendEvt(T &&v) {
        SenderPtr sender = std::make_shared<Sender>(std::move(v));
        
        std::thread t([sender, this]() mutable {
            {
                std::lock_guard<std::mutex> l(this->mutex);
                
                if (!this->closed) {
                    senders.push_back(sender);
                }
                else {
                    sender->e.notify(false);
                }
            }
            
            if (!this->isClosed()) {
                this->m.put(std::move(sender));
            }
        });
        t.detach();
        
        return sender->e;
    }
    
    Event<ReceiveResult> recvEvt() {
        ReceiverPtr receiver = std::make_shared<Receiver>();
         
        std::thread t([receiver, this]() mutable {
            {
                std::lock_guard<std::mutex> l(this->mutex);
                
                if (!this->closed) {
                    receivers.push_back(receiver);
                }
                else {
                    receiver->e.notify(ReceiveResult());
                }
            }
            
            if (!this->isClosed()) {
                // Makes sure that the sender is only taken once.
                SenderPtr sender = this->m.take();
            
                receiver->e.notify(ReceiveResult(std::move(sender->value)));
                sender->e.notify(true);
            }
        });
        t.detach();
        
        return receiver->e;
    }
    
    bool isClosed() {
         std::lock_guard<std::mutex> l(this->mutex);
                
        return this->closed;
    }
    
    void close() {
        {
            std::lock_guard<std::mutex> l(this->mutex);
            closed = true;
        }

        // We do not need to keep the lock here since no thread will modify the lists after changing the flag.
        for (SenderPtr &sender : senders) {
            sender->e.notify(false);
        }
        
        for (ReceiverPtr &receiver : receivers) {
            receiver->e.notify(ReceiveResult());
        }

        senders.clear();
        receivers.clear();
        
        // Joins the putting thread.
        m.tryTake();
    }
    
private:
    struct Sender {
        Sender(T &&value) : value(std::move(value)) {
        }
        Sender(Sender&&) = default;
        
        Event<SendResult> e;
        T value;
    };
    
    struct Receiver {
        Receiver() {
        }
        
        Receiver(Receiver&&) = default;
        
        Event<ReceiveResult> e;
    };
    
    using SenderPtr = std::shared_ptr<Sender>;
    using ReceiverPtr = std::shared_ptr<Receiver>;
    
    std::mutex mutex;
    mvar::MVar<SenderPtr> m;
    std::vector<SenderPtr> senders;
    std::vector<ReceiverPtr> receivers;
    bool closed{false};
};
    
} // namespace events


#endif  // EVENTS_CHANNEL_HPP
