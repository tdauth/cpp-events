# C++ Events

[![Build Status](https://travis-ci.org/tdauth/cpp-events.svg?branch=master)](https://travis-ci.org/tdauth/cpp-events)
[![Code Coverage](https://img.shields.io/codecov/c/github/tdauth/cpp-events/master.svg)](https://codecov.io/github/tdauth/cpp-events?branch=master)

CML-style events for C++.

```cpp
#include <channel.hpp>

int main() {
  events::Channel<std::string> c;
  
  // Thread 1
  std::thread t1([&c] {
    auto e = c.send("Hello world!");
    
    e.sync();
  });
  
  auto e = c.receive();
  e.sync();
  
  t1.join();
  
  return 0;
}
```

## Related Work

* [John Reppy's Publications Concurrent ML](http://people.cs.uchicago.edu/~jhr/papers/cml.html)
* [Transactional Events for ML](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.481.6886&rep=rep1&type=pdf)
* [Transactional Events](https://www.researchgate.net/publication/220676760_Transactional_Events)
* [Boost.Fiber Channels](https://www.boost.org/doc/libs/1_71_0/libs/fiber/doc/html/fiber/synchronization/channels.html)
* [Go-style channels in C++](https://github.com/dragonquest/cpp-channels)
