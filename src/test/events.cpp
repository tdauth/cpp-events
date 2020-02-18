#define BOOST_TEST_MODULE EventsTest

#include <boost/test/included/unit_test.hpp>

#include "../channel.hpp"

BOOST_AUTO_TEST_CASE(ChannelString)
{
	events::Channel<std::string> c;
	
    // Thread 1
	std::thread t1([&c] {
		auto e = c.send("Hello world!");
    
		e.sync();
	});
  
	auto e = c.receive();
	e.sync();
    
    t1.join();
}
