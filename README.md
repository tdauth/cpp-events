# C++ Events

[![Build Status](https://travis-ci.org/tdauth/cpp-events.svg?branch=master)](https://travis-ci.org/tdauth/cpp-events)
[![Code Coverage](https://img.shields.io/codecov/c/github/tdauth/cpp-events/master.svg)](https://codecov.io/github/tdauth/cpp-events?branch=master)

C++ header-only library which provides CML-style events.

CML-style events allow the synchronization of values via channels.
Events are first-class values which are returned by non-blocking send and receive operations.
Only an explicit `sync` call is blocking.
Channels can be closed which prevents any further send and receive operations.
The operations always return a flag which indicates whether the channel has already been closed (similar to Go).

The following example shows how a string value is synchronized between two threads using events:

```cpp
#include <channel.hpp>

int main() {
  events::Channel<std::string> c;
  
  // Thread 1
  std::thread t1([&c] {
    auto e = c.sendEvt("Hello world!");
    
    bool successful = e.sync();
    
    if (!successful) {
      std::cerr << "Channel has been closed." << std::endl;
    }
  });
  
  // Thread 2
  std::thread t2([&c] {
    auto e = c.recvEvt();
    std::optional<std::string> result = e.sync();
  
    if (result.has_value()) {
      std::cout << result.value();
    } else {
      std::cerr << "Channel has been closed." << std::endl;
    }
  });
  
  t1.join();
  t2.join();
  
  return 0;
}
```

Events allow you to use more advanced combinators.
The following example shows how we choose the first synchronized event from two different channels and wrap its result to a string which is printed to the standard output:

```cpp
#include <channel.hpp>

int main() {
  events::Channel<std::string> c1;
  events::Channel<std::string> c2;
  
  // Thread 1
  std::thread t1([&c] {
    c1.sendEvt("A").sync();
  });
  
  // Thread 2
  std::thread t2([&c] {
    c2.sendEvt("B").sync();
  });
  
  // Thread 3
  std::thread t3([&c] {
    auto e1 = c1.recvEvt();
    auto e2 = c2.recvEvt();
    auto e3 = e1.choose(e2);
    auto e4 = e3.wrap<std::string>([] (const std::optional<std::string> &v) {
      if (v.has_value()) {
        return "Result: " + v;
      }
      else {
        return "Channel has been closed.";
      }
    });
    
    std::cout << e4.sync() << std::endl;
  });
  
  t1.join();
  t2.join();
  t3.join();
  
  return 0;
}
```

## Implementation

The current implementation is based on [MVars](https://github.com/tdauth/cpp-mvar).


## Related Work

* [John Reppy's Publications Concurrent ML](http://people.cs.uchicago.edu/~jhr/papers/cml.html)
* [Transactional Events for ML](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.481.6886&rep=rep1&type=pdf)
* [Transactional Events](https://www.researchgate.net/publication/220676760_Transactional_Events)
* [Boost.Fiber Channels](https://www.boost.org/doc/libs/1_71_0/libs/fiber/doc/html/fiber/synchronization/channels.html)
* [Go-style channels in C++](https://github.com/dragonquest/cpp-channels)
