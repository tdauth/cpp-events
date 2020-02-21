#define BOOST_TEST_MODULE EventsTest

#include <boost/test/included/unit_test.hpp>

#include "../channel.hpp"

BOOST_AUTO_TEST_CASE(ChannelString)
{
	events::Channel<std::string> c;
	
    // Thread 1
	std::thread t1([&c] {
		auto e = c.send("Hello world!");
    
        bool result = e.sync();
        
        BOOST_REQUIRE(result);
	});
  
	auto e = c.receive();
	const std::optional<std::string> &result = e.sync();
    
    BOOST_REQUIRE(result.has_value());
    BOOST_REQUIRE_EQUAL(result.value(), "Hello world!");

    t1.join();
}

BOOST_AUTO_TEST_CASE(ChannelStringClosed)
{
	events::Channel<std::string> c;
	c.close();
    
    
    auto e1 = c.receive();
    const std::optional<std::string> &result1 = e1.sync();
    
    BOOST_REQUIRE(!result1.has_value());
    
    
    auto e2 = c.send("Hello world!");
    bool result2 = e2.sync();
    BOOST_REQUIRE(!result2);
}

BOOST_AUTO_TEST_CASE(EventAlways)
{
	events::Event<std::string> e1 = events::always<std::string>("Hello world!");
    BOOST_REQUIRE_EQUAL(e1.sync(), "Hello world!");
    
    events::Event<void> e2 = events::always();
    e2.sync();
}
